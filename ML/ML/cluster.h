#pragma once
#include <string>
#include <map>
#include <memory>

#include "cluster.h"

using namespace std;
using CommonPrefMapT = multimap<string, pair<double, double>>;
class Bicluster
{
private:
	const vector<double> vec, left, right;
	string id;
	double dist;
	double distance(const vector<double>&, const vector<double>&);

	
public:
	Bicluster(unique_ptr<const vector<double>> vec,
		unique_ptr<const vector<double>> left,
		unique_ptr<const vector<double>> right,
			  double distance,
			  const string& id);

	double cluster_pearson(const vector<string>& v1, const vector<string>& v2);
	auto hcluster(const vector<vector<double>>& rows,
		function<double(const vector<double>&, const vector<double>&)> simularity);
};
