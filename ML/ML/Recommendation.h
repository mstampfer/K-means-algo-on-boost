#pragma once
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>

using namespace std;
using PreferenceT = pair<const string, const double>;
using PreferencePairT = pair<const string, PreferenceT>;
using PreferenceMMapT = multimap<const string, PreferenceT>;
using CommonPrefMapT = multimap<string, pair<double, double>>;
using SimilarityPrefMapT = multimap<const string, pair<double, string>>;
using SortedPrefs = map < const double, string, greater<double>>;
typedef multimap<const string, PreferenceT> Critics;
namespace python = boost::python;

class Recommendation
{
private:
	PreferenceMMapT prefs;
	const python::dict critics_p;
	auto itemPairs(const CommonPrefMapT& common);
	PreferenceMMapT SetParameters(const python::dict& critics);

public:
	Recommendation(const python::dict&);
	//Recommendation(PreferenceMMapT& prefs);
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
	python::dict Recommendation::get();
};

