#include <fstream>
#include <regex>
#include <algorithm>
#include <functional>
#include <set>
#include <iostream>

#include "cluster.h"
#include "util.h"
using namespace placeholders;

auto readfile(const string& path, 
	vector<string>& colnames, 
	vector<string>& rownames,
	vector<vector<double>>& data)
{
	regex whitespace("\\s+"), tab("\\t");
	ifstream infile(path + "\\blogdata.txt");
	string line;
	getline(infile, line);
	for_each(sregex_token_iterator(line.cbegin(), line.cend(), tab, -1),
		sregex_token_iterator(), 
		[&colnames](auto it) 
	{ 
		colnames.push_back(it); 
	});

	string::size_type sz;
	while (getline(infile, line))
	{
		vector<double> row;
		bool dataflag = false;
		for_each(sregex_token_iterator(line.cbegin(), line.cend(), tab, -1),
			sregex_token_iterator(), [&](auto it)
		{	
			if (!dataflag)
			{
				rownames.push_back(static_cast<string>(it));
				dataflag = true;
			}
			else
				row.push_back(static_cast<double>(stod(it, &sz)));
		});
		data.push_back(row);
	}
}

Bicluster::Bicluster() :
	id(0), vec(vector<double>()), left(vector<double>()), right(vector<double>()), dist(0.0) {}

Bicluster::Bicluster(const int id, 
	const vector<double>& vec,
	const vector<double>& left = vector<double>(),
	const vector<double>& right = vector<double>(),
	double distance = 0.0)
	 :
	id(id), vec(vec), left(left), right(right), dist(distance)
{}

double Bicluster::cluster_pearson(const vector<double>& v1, const vector<double>& v2)
{
	return Util::pearson(getpairs(v1, v2));
}

double Bicluster::distance(const vector<double>& v1, const vector<double>& v2)
{

	return Util::distance(getpairs(v1, v2));
}

auto Bicluster::hcluster(const vector<vector<double>>& rows,
	function<double(const vector<double>&, const vector<double>&)> simularity

	)
{
	vector<double> mergevec;
	vector<Bicluster> cluster;

	set<unsigned, std::greater<int>> lowestpair;
	vector<unsigned> lowestpairv;
	int currentclustid = -1;

	map<set<unsigned>, double> distances;
	int cid = 0;
	//	Clusters are initially just the rows
	for (const auto& row : rows)
	{
		Bicluster clust(++cid, row);
		cluster.push_back(clust);
	}

	while (cluster.size() > 1)
	{
		lowestpair = { 1, 0 };
		lowestpairv = { 1, 0 };
		auto closest = distance(cluster[0].vec, cluster[1].vec);
		// loop through every pair looking for the smallest distance
		for (unsigned i = 1; i < cluster.size(); ++i)
		{
			for (unsigned j = i + 1; j < cluster.size(); ++j)
			{
				cout << "i " << i << " j " << j << " " << cluster.size() << endl;
				// distances is the cache of distance calculations
				set<unsigned> test{ i, j };
				if (distances.find(test) == distances.cend())
					distances[test] = distance(cluster[i].vec, cluster[j].vec);
				double d = distances[test];
				if (d < closest)
				{
					closest = d;
					lowestpair = { i, j };
					lowestpairv = { i, j };
				}
			}
			// calculate the average of the two clusters
			for (unsigned idx = 0; idx < cluster[0].vec.size(); ++idx)
			{
				mergevec.push_back((cluster[lowestpairv[0]].vec[idx]
					+ cluster[lowestpairv[1]].vec[idx]) / 2.0);
			}
			//create the new cluster
			Bicluster newcluster(currentclustid,
				mergevec,
				cluster[lowestpairv[0]].vec,
				cluster[lowestpairv[1]].vec,
				closest
				);
			//cluster ids that weren't in the original set are negative
			currentclustid -= 1;
			for (const auto& e : lowestpair)
				cluster.erase(cluster.begin() + e);
			cluster.push_back(newcluster);
		}
	}
	return cluster[0];
}

void Bicluster::printclust(const vector<Bicluster>& cluster, const vector<string>& labels, unsigned n) 
{
	// indent to make a hierarchy layout
	for (int i = 0; i != n; ++i)
	{
		cout << ' ';
		if (cluster[i].id < 0)
			// negative id means that this is branch
			cout << '-' << endl;
		else
		{
			// positive id means that this is an endpoint
			if (labels.size() == 0)
				cout << cluster[i].id << endl;
			else
				cout << labels[cluster[i].id] << endl;
		}
		// now print the right and left branches
		//if (cluster[i].left.size > 0)
		//	printclust(cluster[i].left, labels, n + 1);
		//if (cluster[i].right.size > 0)
		//	printclust(cluster[i].right, labels, n + 1);
	}
}
int main()
{
	const string& path("C:\\work\\MachineLearning2");
	vector<string> colnames, rownames;
	vector<vector<double>> data;
	readfile(path, colnames, rownames, data);
	Bicluster cluster;
	auto similarity = bind(&Bicluster::cluster_pearson, cluster, _1, _2);
	auto clusters = cluster.hcluster(data, similarity);

//	clusters.printclust(clusters, rownames);
}