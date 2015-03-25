#include <fstream>
#include <regex>
#include <algorithm>
#include <functional>
#include <set>

#include "cluster.h"
#include "util.h"

auto readfile(const string& path, 
	vector<string>& colnames, 
	vector<string>& rownames,
	vector<vector<string>>& data)
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

	while (getline(infile, line))
	{
		vector<string> row;
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
				row.push_back(it);
		});
		data.push_back(row);
	}
}

Bicluster::Bicluster(unique_ptr<const vector<double>> vec,
	unique_ptr<const vector<double>> left = nullptr,
	unique_ptr<const vector<double>> right = nullptr,
	double distance = 0.0,
	const string& id = nullptr) :
	vec(*vec.get()), left(*left.get()), right(*right.get()),  id(id), dist(distance)
{}


double Bicluster::cluster_pearson(const vector<string>& v1, const vector<string>& v2) 
{
	return 0.0;
}
double Bicluster::distance(const vector<double>& v1, const vector<double>& v2)
{
	vector<pair<double, double>> pairs;
	for (int i = 0; i != v1.size(); ++i)
	{
		pairs.push_back(make_pair(v1[i], v2[i]));
	}
	return Util::distance(pairs);
}
auto Bicluster::hcluster(const vector<vector<double>>& rows,
	function<double(const vector<double>&, const vector<double>&)> simularity)
{
	vector<Bicluster> cluster;
	vector<double> mergevec;
	set<unsigned, std::greater<int>> lowestpair{1, 0};
	vector<unsigned> lowestpairv{ 0, 1 };
	int currentclustid = -1;

	map<set<unsigned>, double> distances;
	//	Clusters are initially just the rows
	for (const auto& row : rows)
	{
		string i;
		Bicluster clust(make_unique<const vector<double>>(row), nullptr, nullptr, 0.0, i);
		cluster.push_back(clust);
	}
	auto closest = distance(cluster[0].vec, cluster[1].vec);

	// loop through every pair looking for the smallest distance
	for (unsigned i = 1; i <= cluster.size(); ++i)
	{
		for (unsigned j = 1; i <= cluster.size(); ++j)
		{
		// distances is the cache of distance calculations
			set<unsigned> test{ i, j };
			if (distances.find(test) == distances.cend())
				distances[test] = distance(cluster[i].vec, cluster[j].vec);
			double d = distances[test];
			if (d < closest)
			{
				closest = d;
				lowestpair  = { i, j };
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
		Bicluster newcluster(make_unique<const vector<double>>(mergevec),
			make_unique<const vector<double>>(cluster[lowestpairv[0]]),
			make_unique<const vector<double>>(cluster[lowestpairv[1]],
			closest,
			currentclustid));
		//cluster ids that weren't in the original set are negative
		currentclustid -= 1;
		for (const auto& e : lowestpair)
			cluster.erase(cluster.begin() + e);
		cluster.push_back(newcluster);
		return cluster[0];
	}
}

//	mergevec = [
//		(clust[lowestpair[0]].vec[i] + clust[lowestpair[1]].vec[i]) / 2.0
//			for i in range(len(clust[0].vec))]
//				# create the new cluster
//				newcluster = bicluster(mergevec, left = clust[lowestpair[0]],
//				right = clust[lowestpair[1]],
//				distance = closest, id = currentclustid)
//				# cluster ids that weren't in the original set are negative
//				currentclustid -= 1
//				del clust[lowestpair[1]]
//				del clust[lowestpair[0]]
//				clust.append(newcluster)
//				return clust[0]

int main()
{
	const string& path("C:\\work\\MachineLearning2");
	vector<string> colnames, rownames;
	vector<vector<string>> data;
	readfile(path, colnames, rownames, data);
}