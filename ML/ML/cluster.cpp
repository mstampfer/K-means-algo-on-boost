#include <fstream>
#include <regex>
#include <algorithm>
#include <functional>

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

auto Bicluster::hcluster(const vector<vector<double>>& rows,
	function<double(const vector<double>&, const vector<double>&)> simularity)
{
	int currentclustid = -1;
	vector<Bicluster> cluster;
	//	Clusters are initially just the rows
	for (const auto& row : rows)
	{
		string i;
		Bicluster clust(make_unique<const vector<double>>(row), nullptr, nullptr, 0.0, i);
		cluster.push_back(clust);
	}
	auto closest = distance(cluster[0].vec, cluster[1].vec);
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
//def hcluster(rows, distance = pearson) :
//	distances = {}
//	currentclustid = -1
//	Clusters are initially just the rows
//	clust = [bicluster(rows[i], id = i) for i in range(len(rows))]
//	while len(clust)>1:
//lowestpair = (0, 1)
//closest = distance(clust[0].vec, clust[1].vec)
//# loop through every pair looking for the smallest distance
//for i in range(len(clust)) :
//	for j in range(i + 1, len(clust)) :
//		# distances is the cache of distance calculations
//		if (clust[i].id, clust[j].id) not in distances :
//distances[(clust[i].id, clust[j].id)] = distance(clust[i].vec, clust[j].vec)
//d = distances[(clust[i].id, clust[j].id)]
//if d<closest :
//	closest = d
//	lowestpair = (i, j)
//	# calculate the average of the two clusters
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