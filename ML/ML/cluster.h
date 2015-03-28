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
	vector<double> vec;
	unique_ptr<Bicluster> left, right;
	int id;
	double dist;
	double distance(const vector<double>&, const vector<double>&);

	function<vector<pair<double, double>>(const vector<double>&, const vector<double>&)>
		getpairs = [](const vector<double>& v1, const vector<double>& v2)
	{
		vector<pair<double, double>> pairs;
		for (int i = 0; i != v1.size(); ++i)
		{
			pairs.push_back(make_pair(v1[i], v2[i]));
		}
		return pairs;
	};

	function<vector<vector<double>>(vector<vector<double>>)> transpose  
		= [](const vector<vector<double>>& a)
	{
		vector<vector<double>> b(a[0].size(), vector<double>(a.size()));
		for (unsigned i = 0; i < a.size(); ++i)
			for (unsigned j = 0; j < a[0].size(); ++j)
				b[j][i] = a[i][j];
		return b;
	};

public:
	Bicluster();
	Bicluster(const int id,
		const vector<double>& vec,
		unique_ptr<Bicluster> left,
		unique_ptr<Bicluster> right,
		double distance);

	double cluster_pearson(const vector<double>& v1, const vector<double>& v2);
	auto hcluster(const vector<vector<double>>& rows,
		function<double(const vector<double>&, const vector<double>&)> simularity);
	auto kcluster(const vector<vector<double>>& rows,
		function<double(const vector<double>&, const vector<double>&)> simularity, unsigned n);
	void printclust(const Bicluster& cluster, const vector<string>& labels, unsigned n);
};
