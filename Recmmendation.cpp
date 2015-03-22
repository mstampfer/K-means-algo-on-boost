#include <map>
#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <set>
#include <algorithm>
#include <functional>
#include <fstream>
#include <regex>

#include "Recommendation.h"

Recommendation::Recommendation(PreferenceMMapT& prefs) : prefs(prefs) {}

auto Recommendation::find_common(const string &person1, const string &person2)
{
	CommonPrefMapT common_items;
	const auto p1_items = prefs.equal_range(person1);
	const auto p2_items = prefs.equal_range(person2);
	vector<PreferencePairT> person1v(p1_items.first, p1_items.second);
	vector<PreferencePairT> person2v(p2_items.first, p2_items.second);

	for (const auto& e1 : person1v)
	{
		for (const auto& e2 : person2v)
		{
			const auto& e1pair = e1.second; const auto& e2pair = e2.second;
			if (e1pair.first == e2pair.first)
			{
				const auto& p = make_pair(e1pair.second, e2pair.second);
				common_items.insert(pair<string, decltype(p)>(e1.first, p));
			}
		}
	}
	return common_items;
}

auto Recommendation::sim_distance(
	const string& person1,
	const string& person2)
{
	auto sum_of_squares = 0.0;
	const auto& common = find_common(person1, person2);
	for_each(common.cbegin(), common.cend(), [&sum_of_squares](const auto& item) {
		sum_of_squares += pow(item.second.first - item.second.second, 2);
	});
	if (sum_of_squares == 0)
		return 0.0;
	return 1.0 / (1.0 + sum_of_squares);
}


//Returns the Pearson correlation coefficient for p1 and p2
auto Recommendation::sim_pearson(const string& person1, const string& person2)
{

	// Get the list of mutually rated items
	const auto& common = find_common(person1, person2);
	if (common.size() == 0.0)
		return 0.0;

	//       Add up all the preferences
	auto sum1 = 0.0, sum2 = 0.0;
	for_each(common.cbegin(), common.cend(), [&sum1, &sum2](const auto& prefMap) {
		sum1 += prefMap.second.first;
		sum2 += prefMap.second.second;
	});

	//       Sum up the squares
	auto sum1Sq = 0.0, sum2Sq = 0.0;
	for_each(common.cbegin(), common.cend(), [&sum1Sq, &sum2Sq](const auto& prefMap) {
		sum1Sq += pow(prefMap.second.first, 2);
		sum2Sq += pow(prefMap.second.second, 2);
	});

	//       Sum up the products
	auto pSum = 0.0;
	for_each(common.cbegin(), common.cend(), [&pSum](const auto& prefMap) {
		pSum += prefMap.second.first*prefMap.second.second;
	});

	//       Calculate Pearson score
	const auto& n = common.size();
	const auto& num = pSum - (sum1*sum2 / n);
	const auto& den = sqrt((sum1Sq - pow(sum1, 2) / n)*(sum2Sq - pow(sum2, 2) / n));
	if (den == 0) return 0.0;
	const auto& r = num / den;
	return r;
}

// Returns the best matches for person from the prefs dictionary.
// Number of results and similarity function are optional params.
auto Recommendation::topMatches(
	const string &person,
	function<double(
	const string &person1,
	const string &person2)> similarity)
{
	map<double, string, greater<double>> scores;
	for_each(prefs.cbegin(), prefs.cend(), [&scores, &similarity, &person](const auto& pref) {
		if (pref.first != person)
			scores.insert(make_pair(similarity(pref.first, person), pref.first));
	});
	return scores;
}

auto Recommendation::all_names()
{
	set<string> names;
	for (auto &pref : prefs)
	{
		names.insert(pref.first);
	}
	return names;
}

auto Recommendation::all_movies(const string& person)
{
	set<string> movies;
	const auto& p = prefs.equal_range(person);

	for (auto it = p.first; it != p.second; ++it)
	{
		movies.insert(it->second.first);
	}
	return movies;
}

// Gets recommendations for a person by using a weighted average
// of every other user's rankings
auto Recommendation::getRecommendations(
	const string &person,
	function<double(
	const string &person1,
	const string &person2)> similarity)
{
	const auto& names = all_names();
	map<string, double> totals;
	map<string, double> simSums;
	const auto& personprefs = prefs.equal_range(person);
	const auto& personmovies = all_movies(person);

	for (auto &name : names)
	{
		if (name != person) // don't compare me to myself
		{
			const auto& sim = similarity(person, name);
			// ignore scores of zero or lower
			if (sim > 0)
			{

				const auto& namedprefs = prefs.equal_range(name);
				for (auto it = namedprefs.first; it != namedprefs.second; ++it)
				{

					// only score movies I haven't seen yet
					if (personmovies.find(it->second.first) == personmovies.cend())
					{
						// Similarity * Score
						totals[it->second.first] += it->second.second * sim;
						//Sum of similarities
						simSums[it->second.first] += sim;
					}

				}
			}

		}
	}
	// Create the normalized list
	map<double, string, greater<double>> rankings;
	for (auto &item : totals)
	{
		rankings[item.second / simSums[item.first]] = item.first;
	}
	//# Return the inverse sorted list
	return rankings;
}

auto Recommendation::transformPrefs()
{
	PreferenceMMapT result;
	for (auto &pref : prefs)
	{
		result.insert(make_pair(pref.second.first,
			make_pair(pref.first, pref.second.second)));
	}
	prefs = result;
}

auto Recommendation::calculateSimilarItems()
{
	// Create a dictionary of items showing which other items they are most similar to.
	SimilarityPrefMapT result;
	// Invert the preference matrix to be item-centric
	transformPrefs();
	int c = 0;
	for (auto &item : prefs)
	{
		// Status updates for large datasets
		++c;
		if (c % 100 == 0.0)
			cout << c << "/" << prefs.size() << endl;
		// Find the most similar items to this one
		const auto& scores = this->topMatches(item.first, sim_pearson);
		for (auto &score : scores)
		{
			result.insert(make_pair(item.first, score));
		}
	}
	return result;
}

auto Recommendation::getRecommendedItems(
	const SimilarityPrefMapT &itemMatch,
	const string &user)
{
	map<string, double> scores;
	map<string, double> totalSim;

	// Return the rankings from highest to lowest rankings.sort( )
	map<double, string, greater<double>> rankings;

	const auto& userRatingsMap = prefs.equal_range(user);
	const auto& usermovies = all_movies(user);


	// Loop over items rated by this user
	for (auto userRating = userRatingsMap.first; userRating != userRatingsMap.second; ++userRating)
	{
		const auto& item = userRating->second.first;
		const auto& rating = userRating->second.second;

		// Loop over items similar to this one
		const auto& similarityMap = itemMatch.equal_range(item);
		for (auto similarityRating = similarityMap.first; similarityRating != similarityMap.second; ++similarityRating)
		{
			const auto& similarity = similarityRating->second.first;
			const auto& item2 = similarityRating->second.second;

			// Ignore if this user has already rated this item
			if (usermovies.find(item2) == usermovies.cend())
			{

				// Weighted sum of rating times similarity
				scores[item2] += similarity * rating;

				// Sum of all the similarities
				totalSim[item2] += similarity;
			}
		}
	}

	// Divide each total score by total weighting to get an average
	for (auto &score : scores)
	{
		const auto& item = score.first;
		const auto& rank = score.second;
		rankings.insert(make_pair(rank / totalSim[item], item));
	}

	return rankings;
}