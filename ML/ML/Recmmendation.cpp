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
#include "util.h"

//Recommendation::Recommendation(PreferenceMMapT& prefs) : prefs(prefs) {}

enum struct Recommendation::Similarity { sim_distance, sim_pearson };

Recommendation::Recommendation(const python::dict& critics)
	: critics_p(critics)
{
}

PreferenceMMapT Recommendation::SetParameters(const python::dict& vals)
{
	Critics critics;
	python::list keys = vals.keys();

	for (int i = 0; i < len(keys); ++i)
	{
		auto curArg = static_cast<python::dict>(vals[keys[i]]);
		if (curArg)
		{
			string key(python::extract<string>(vals.keys()[i]));
			for (int j = 0; j < len(curArg); ++j)
			{
				string s = move(python::extract<string>(curArg.keys()[j]));
				double  d = move(python::extract<double>(curArg.values()[j]));
				critics.insert(make_pair(key, make_pair(s, d)));
			}
		}
	}

	//for (auto& e1 : critics)
	//	cout << e1.first << " " << e1.second.first << " " << e1.second.second << endl;

	return critics;
}

python::dict Recommendation::get()
{
	return critics_p;
}

auto Recommendation::Func(const Similarity& sim)
{
	if (sim == Similarity::sim_distance)
	{
		return bind(&Recommendation::sim_distance, *this, _1, _2);
	}
	if (sim == Similarity::sim_pearson)
	{
		return bind(&Recommendation::sim_pearson, *this, _1, _2);
	}
}

auto Recommendation::find_common(const string &person1, const string &person2)
{
	CommonPrefMapT common_items;
	prefs = SetParameters(get());
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

auto Recommendation::itemPairs(const CommonPrefMapT& common)
{
	vector<pair<double, double>> items;
	for_each(common.cbegin(), common.cend(), [&items](const auto& item) 
	{
		items.push_back(make_pair(item.second.first, item.second.second));
	});
	return items;
}

double Recommendation::sim_distance(
	const string& person1,
	const string& person2)
{
	const auto& common = find_common(person1, person2);
	const auto items = itemPairs(common);
	return Util::distance(items);
}

//Returns the Pearson correlation coefficient for p1 and p2
double Recommendation::sim_pearson(const string& person1, const string& person2)
{

	// Get the list of mutually rated items
	const auto& common = find_common(person1, person2);
	if (common.size() == 0.0)
		return 0.0;
	const auto items = itemPairs(common);
	return Util::pearson(items);
}

 //Returns the best matches for person from the prefs dictionary.
 //Number of results and similarity function are optional params.
SortedPrefs Recommendation::topMatches(
	const string &person,
	function<double(
	const string &person1,
	const string &person2)> similarity)
{
	SortedPrefs scores;
	for_each(prefs.cbegin(), prefs.cend(), [&scores, &similarity, &person](const auto& pref) {
		auto f_sim = bind(similarity, pref.first, person);
		if (pref.first != person)
			scores.insert(make_pair(f_sim(), pref.first));
	});
	return scores;
}

python::dict Recommendation::topMatchesWrapper(
	const string& person,
	const Similarity& similarity)
{
	python::dict d;
	auto scores = topMatches(person, Func(similarity));
	for_each(scores.cbegin(), scores.cend(), [&d](auto& score){
		d[score.first] = score.second;
	});
	return d;
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
SortedPrefs Recommendation::getRecommendations(
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
			auto f_sim = bind(similarity, person, name);
			auto sim = f_sim();
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
	SortedPrefs rankings;
	for (auto &item : totals)
	{
		rankings[item.second / simSums[item.first]] = item.first;
	}
	//# Return the inverse sorted list
	return rankings;
}

python::dict Recommendation::getRecommendationsWrapper(const string& person, const Similarity& sim)
{
	python::dict d;
	auto rankings = getRecommendations(person, Func(sim));
	for_each(rankings.cbegin(), rankings.cend(), [&d](auto &rank) {
		d[rank.first] = rank.second;
	});
	return d;
}

void Recommendation::transformPrefs()
{
	PreferenceMMapT result;
	for (auto &pref : prefs)
	{
		result.insert(make_pair(pref.second.first,
			make_pair(pref.first, pref.second.second)));
	}
	prefs = result;
}

SimilarityPrefMapT Recommendation::calculateSimilarItems()
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
		//const auto& scores = this->topMatches(item.first);
		//for (auto &score : scores)
		//{
		//	result.insert(make_pair(item.first, score));
		//}
	}
	return result;
}

SortedPrefs Recommendation::getRecommendedItems(
	const SimilarityPrefMapT &itemMatch,
	const string &user)
{
	map<string, double> scores;
	map<string, double> totalSim;

	// Return the rankings from highest to lowest rankings.sort( )
	SortedPrefs rankings;

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

BOOST_PYTHON_MODULE(ml_ext)
{ 
	python::class_<Recommendation>("Recommendation", 
		python::init<const python::dict&>())
		.def("find_common", &Recommendation::find_common)
		.def("sim_distance", &Recommendation::sim_distance)
		.def("sim_pearson", &Recommendation::sim_pearson)
		.def("topMatches", &Recommendation::topMatchesWrapper)
		.def("all_names", &Recommendation::all_names)
		.def("all_movies", &Recommendation::all_movies)
		.def("getRecommendations", &Recommendation::getRecommendationsWrapper)
		.def("transformPrefs", &Recommendation::transformPrefs)
		.def("calculateSimilarItems", &Recommendation::calculateSimilarItems)
		.def("getRecommendedItems", &Recommendation::getRecommendedItems)
		.add_property("critics_p", &Recommendation::get)
		;
	python::enum_<Recommendation::Similarity>("Similarity")
		.value("distance", Recommendation::Similarity::sim_distance)
		.value("sim_pearson", Recommendation::Similarity::sim_pearson)
		;
}