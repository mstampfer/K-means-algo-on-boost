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

auto loadMovieLens(const string& path)
{
	string line;
	string::size_type sz;
	map<int, string> movies;
	regex re("\\|");
	ifstream infile(path + "\\u.item");
	while (getline(infile, line))
	{
		for (auto it = sregex_token_iterator(line.cbegin(), line.cend(), re, -1); it != sregex_token_iterator(); )
		{
			const auto& id = stoi(static_cast<string>(*it++), &sz);
			const auto& title = static_cast<string>(*it++);
			movies[id] = title;
			break;
		}
	}

	regex re2("\t");
	ifstream infile2(path + "\\u.data");
	PreferenceMMapT prefs;
	while (getline(infile2, line))
	{
		for (auto it = sregex_token_iterator(line.cbegin(), line.cend(), re2, -1); it != sregex_token_iterator(); )
		{
			const auto& user = static_cast<string>(*it++);
			const auto& id = stoi(static_cast<string>(*it++), &sz);
			const auto& rating = stod(static_cast<string>(*it++), &sz);
			prefs.insert(make_pair(user, make_pair(movies[id], rating)));
			break;
		}
	}
	return prefs;
}


int main() {

	PreferenceMMapT critics = {
		{ "Lisa Rose",{ "Lady in the Water", 2.5 } },
		{ "Lisa Rose",{ "Snakes on a Plane", 3.5 } },
		{ "Lisa Rose",{ "Just My Luck", 3.0 } },
		{ "Lisa Rose",{ "Superman Returns", 3.5 } },
		{ "Lisa Rose",{ "You, Me and Dupree", 2.5 } },
		{ "Lisa Rose",{ "The Night Listener", 3.0 } },
		{ "Gene Seymour",{ "Lady in the Water", 3.0 } },
		{ "Gene Seymour",{ "Snakes on a Plane", 3.5 } },
		{ "Gene Seymour",{ "Just My Luck", 1.5 } },
		{ "Gene Seymour",{ "Superman Returns", 5.0 } },
		{ "Gene Seymour",{ "The Night Listener", 3.0 } },
		{ "Gene Seymour",{ "You, Me and Dupree", 3.5 } },
		{ "Michael Phillips",{ "Lady in the Water", 2.5 } },
		{ "Michael Phillips",{ "Snakes on a Plane", 3.0 } },
		{ "Michael Phillips",{ "Superman Returns", 3.5 } },
		{ "Michael Phillips",{ "The Night Listener", 4.0 } },
		{ "Claudia Puig",{ "Snakes on a Plane", 3.5 } },
		{ "Claudia Puig",{ "Just My Luck", 3.0 } },
		{ "Claudia Puig",{ "The Night Listener", 4.5 } },
		{ "Claudia Puig",{ "Superman Returns", 4.0 } },
		{ "Claudia Puig",{ "You, Me and Dupree", 2.5 } },
		{ "Mick LaSalle",{ "Lady in the Water", 3.0 } },
		{ "Mick LaSalle",{ "Snakes on a Plane", 4.0 } },
		{ "Mick LaSalle",{ "Just My Luck", 2.0 } },
		{ "Mick LaSalle",{ "Superman Returns", 3.0 } },
		{ "Mick LaSalle",{ "The Night Listener", 3.0 } },
		{ "Mick LaSalle",{ "You, Me and Dupree", 2.0 } },
		{ "Jack Matthews",{ "Lady in the Water", 3.0 } },
		{ "Jack Matthews",{ "Snakes on a Plane", 4.0 } },
		{ "Jack Matthews",{ "The Night Listener", 3.0 } },
		{ "Jack Matthews",{ "Superman Returns", 5.0 } },
		{ "Jack Matthews",{ "You, Me and Dupree", 3.5 } },
		{ "Toby",{ "Snakes on a Plane", 4.5 } },
		{ "Toby",{ "You, Me and Dupree", 1.0 } },
		{ "Toby",{ "Superman Returns", 4.0 } } };

	Recommendation rec(critics);

	    cout << rec.sim_distance("Lisa Rose", "Gene Seymour") << endl;
	
	    cout << rec.sim_pearson("Lisa Rose", "Gene Seymour") << endl;
	
	    auto scores = rec.topMatches("Toby", rec.sim_distance);
	    for_each(scores.cbegin(), scores.cend(), [](auto& score){
	        cout << score.first << ", " << score.second << endl;
	    });

	    auto rankings = rec.getRecommendations( "Toby", rec.sim_pearson);
	    for_each(rankings.cbegin(), rankings.cend(), [](auto &rank) {
	        cout << rank.first << ", " << rank.second << endl;
	    });

	    auto movies = rec.transformPrefs();
	    auto topMatch = rec.topMatches(movies, "Superman Returns");
	    for_each(topMatch.cbegin(), topMatch.cend(), [](auto &score) {
	        cout << score.first << ", " << score.second << endl;
	    });


	const auto& itemsIm = rec.calculateSimilarItems();
	for_each(itemsIm.cbegin(), itemsIm.cend(), [](auto &item) {
	    cout << item.first << ", " << item.second.first << ", " << item.second.second << endl;
	});

	auto rankings = rec.getRecommendedItems( itemsIm, "Toby" );
	for(auto& ranking : rankings)
	{
	    cout << ranking.first << " " <<  ranking.second << " " << endl;
	}

	const string& path("C:\\work\\MachineLearning2\\ml-100k");
	const auto& prefs = loadMovieLens(path);
	auto test =  prefs.equal_range("87");
	for_each(test.first, test.second, [](const auto& test) {
		cout << test.first << ", " << test.second.first << ", " << test.second.second << endl;
	});

	auto recs = rec.getRecommendations("87");
	for (const auto& rec : recs)
	{
		cout << rec.first << ", " << rec.second << endl;
	}

	const auto& itemSim = rec.calculateSimilarItems();
	const auto& recs = rec.getRecommendedItems(itemSim, "87");
	for (const auto& rec : recs)
	{
		cout << rec.first << ", " << rec.second << endl;
	}
	return 0;
}

