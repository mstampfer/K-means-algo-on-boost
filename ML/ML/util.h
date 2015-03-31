#pragma once
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <functional>
#include <numeric>

using namespace std;
using CommonPrefMapT = multimap<string, pair<double, double>>;

class Util
{
public:

	static double pearson(const vector<pair<double, double>>& common);
	static double distance(const vector<pair<double, double>>& common);
	static vector<unsigned> range(unsigned len, unsigned start = 0);
};
