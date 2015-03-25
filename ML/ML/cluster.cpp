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
	id(0), vec(vector<double>()), left(nullptr), right(nullptr), dist(0.0) {}

Bicluster::Bicluster(const int id, 
	const vector<double>& vec,
	unique_ptr<Bicluster> left = nullptr,
	unique_ptr<Bicluster> right = nullptr,
	double distance = 0.0)
	 :
	id(id), vec(vec), left(move(left)), right(move(right)), dist(distance)
{}

double Bicluster::cluster_pearson(const vector<double>& v1, const vector<double>& v2)
{
	return Util::pearson(getpairs(v1, v2));
}

double Bicluster::distance(const vector<double>& v1, const vector<double>& v2)
{

	return Util::distance(getpairs(v1, v2));
}

auto& Bicluster::hcluster(const vector<vector<double>>& rows,
	function<double(const vector<double>&, const vector<double>&)> simularity

	)
{
	vector<double> mergevec;
	vector<Bicluster> clusters;

	set<unsigned, std::greater<int>> lowestpair;
	vector<unsigned> lowestpairv;
	int currentclustid = -1;

	map<set<unsigned>, double> distances;
	int cid = 0;
	//	Clusters are initially just the rows
	for (const auto& row : rows)
	{
		//Bicluster clust(++cid, row);
		clusters.emplace_back(++cid, row);
	}

	while (clusters.size() > 1)
	{
		lowestpair = { 1, 0 };
		lowestpairv = { 1, 0 };
		auto closest = distance(clusters[0].vec, clusters[1].vec);
		// loop through every pair looking for the smallest distance
		for (unsigned i = 1; i < clusters.size(); ++i)
		{
			for (unsigned j = i + 1; j < clusters.size(); ++j)
			{
				cout << "i " << i << " j " << j << " " << clusters.size() << endl;
				// distances is the cache of distance calculations
				set<unsigned> test{ i, j };
				if (distances.find(test) == distances.cend())
					distances[test] = distance(clusters[i].vec, clusters[j].vec);
				double d = distances[test];
				if (d < closest)
				{
					closest = d;
					lowestpair = { i, j };
					lowestpairv = { i, j };
				}
			}
			// calculate the average of the two clusters
			for (unsigned idx = 0; idx < clusters[0].vec.size(); ++idx)
			{
				mergevec.push_back((clusters[lowestpairv[0]].vec[idx]
					+ clusters[lowestpairv[1]].vec[idx]) / 2.0);
			}
			//create the new cluster
			//Bicluster newcluster(currentclustid,
			//	mergevec,
			//	make_unique<Bicluster>(move(clusters[lowestpairv[0]])),
			//	make_unique<Bicluster>(move(clusters[lowestpairv[1]])),
			//	closest
			//	);
			//cluster ids that weren't in the original set are negative
			currentclustid -= 1;
			for (const auto& e : lowestpair)
				clusters.erase(clusters.begin() + e);
			clusters.emplace_back(currentclustid,
				mergevec,
				make_unique<Bicluster>(move(clusters[lowestpairv[0]])),
				make_unique<Bicluster>(move(clusters[lowestpairv[1]])),
				closest);
		}
	}
	return move(clusters[0]);
}

void Bicluster::printclust(const Bicluster& cluster, const vector<string>& labels, unsigned n) 
{
	// indent to make a hierarchy layout
	for (unsigned i = 0; i < n; ++i)
	{
		cout << ' ';
		if (cluster.id < 0)
			// negative id means that this is branch
			cout << '-' << endl;
		else
		{
			// positive id means that this is an endpoint
			if (labels.size() == 0)
				cout << cluster.id << endl;
			else
				cout << labels[cluster.id] << endl;
		}
		 //now print the right and left branches
		if (cluster.left->vec.size() > 0)
			printclust(*cluster.left, labels, n + 1);
		if (cluster.right->vec.size() > 0)
			printclust(*cluster.right, labels, n + 1);
	}
}
int main()
{
	const string& path("C:\\work\\MachineLearning2");
	vector<string> colnames, rownames;
	vector<vector<double>> data;
	readfile(path, colnames, rownames, data);
	Bicluster cluster;
	//auto& similarity = bind(&Bicluster::cluster_pearson, cluster, _1, _2);
	auto& clusters = cluster.hcluster(data, bind(&Bicluster::cluster_pearson, cluster, _1, _2));

	clusters.printclust(clusters, rownames);
}