#pragma once
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>

using namespace std;
using namespace boost::python;
using PreferenceT = pair<const string, const double>;
using PreferencePairT = pair<const string, PreferenceT>;
using PreferenceMMapT = multimap<const string, PreferenceT>;
using CommonPrefMapT = multimap<string, pair<double, double>>;
using SimilarityPrefMapT = multimap<const string, pair<double, string>>;
using SortedPrefs = map < const double, string, greater<double>>;

class Recommendation
{
private:
	PreferenceMMapT& prefs;
	auto itemPairs(const CommonPrefMapT& common);
public:
	Recommendation(PreferenceMMapT& prefs);
	auto find_common(const string &person1, const string &person2);
	double sim_distance(const string& person1, const string& person2);
	double sim_pearson(const string& person1, const string& person2);
	SortedPrefs topMatches(const string &person,
		function<double(
		const string &person1,
		const string &person2)> similarity = &sim_pearson);
	auto all_names();
	auto all_movies(const string& person);
	SortedPrefs getRecommendations(const string &person,
		function<double(
		const string &person1,
		const string &person2)> similarity);

	void transformPrefs();
	SimilarityPrefMapT calculateSimilarItems();
	SortedPrefs getRecommendedItems(const SimilarityPrefMapT &itemMatch,
		const string &user);
	static auto buildDict();
};

struct World
{
	World(std::string msg);
	void set(std::string msg);
	std::string greet();
	std::string msg;
};
