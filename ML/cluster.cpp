#include <fstream>
#include <regex>
#include <algorithm>
#include <functional>
#include <set>
#include <iostream>
#include <random>
#include <list>

#include "cluster.h"
#include "Recommendation.h"

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

Bicluster::Bicluster(const unique_ptr<Bicluster>& rhs) :
	id(rhs->id), 
	vec(rhs->vec), 
	left(new Bicluster(rhs->left)), 
	right(new Bicluster(rhs->right)), 
	dist(rhs->dist) {}

Bicluster::Bicluster(const int id,
	const vector<double>& vec,
	unique_ptr<Bicluster> left = nullptr,
	unique_ptr<Bicluster> right = nullptr,
	double distance = 0.0)
	 :
	id(id), vec(vec), left(move(left)), right(move(right)), dist(distance)
{}

Bicluster::Bicluster(const Bicluster& rhs) : 
	vec(rhs.vec) ,
	id(rhs.id),
	dist(rhs.dist),
	left(new Bicluster(rhs.left)),
	right(new Bicluster(rhs.right))
{
}

Bicluster& Bicluster::operator=(const Bicluster& rhs) 
{
	vec = rhs.vec;
	left = make_unique<Bicluster>(rhs.left);
	right = make_unique<Bicluster>(rhs.right);
	id = rhs.id;
	dist = rhs.dist;
	return *this;
}

// boost-python does not support move semantics
Bicluster::Bicluster(Bicluster&&rhs ) :
	vec(move(rhs.vec)),
	left(move(rhs.left)),
	right(move(rhs.right)),
	id(move(rhs.id)),
	dist(move(rhs.dist))
{}

Bicluster& Bicluster::operator=(Bicluster&& rhs) 
{
	vec = move(rhs.vec);
	left = move(rhs.left);
	right = move(rhs.right);
	id = move(rhs.id);
	dist = move(rhs.dist);
	return *this;
}

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
	vector<Bicluster> clusters;
	set<unsigned, std::greater<int>> lowestpair;
	vector<unsigned> lowestpairv;
	int currentclustid = -1;
	int cid = 0;

	//	Clusters are initially just the rows
	for (const auto& row : rows)
	{
		Bicluster bc(cid++, row);
		clusters.push_back(move(bc));
	}

	while (clusters.size() > 1)
	{
		lowestpair = { 1, 0 };
		lowestpairv = { 1, 0 };
		map<set<unsigned>, double> distances;
		auto closest = distance(clusters[0].vec, clusters[1].vec);
	
		// loop through every pair looking for the smallest distance
		for (auto i : Util::range(clusters.size()))
		{
			for (auto j : Util::range(clusters.size(), i + 1))
			{
				//cout << "i " << i << " j " << j << " " << clusters.size() << endl;
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
		}
		// calculate the average of the two clusters
		if (lowestpair.size() > 0)
		{
			for (auto idx : Util::range(clusters[0].vec.size()))
			{
				mergevec.push_back((clusters[lowestpairv[0]].vec[idx]
					+ clusters[lowestpairv[1]].vec[idx]) / 2.0);
			}

			//create the new cluster
			Bicluster bc(currentclustid,
				mergevec,
				make_unique<Bicluster>(move(clusters[lowestpairv[0]])),
				make_unique<Bicluster>(move(clusters[lowestpairv[1]])),
				closest);
			
			clusters.push_back(move(bc));

			//remove merged clusters
			for (const auto& e : lowestpair)
			{
				cout << e << " ";
				clusters.erase(clusters.begin() + e);
			}
			cout << lowestpair.size() << endl;
			lowestpair = {};

			//cluster ids that weren't in the original set are negative
			currentclustid -= 1;
		}
	}
	return move(clusters[0]);
}

void Bicluster::printclust(const Bicluster& cluster, const vector<string>& labels, unsigned n = 0) 
{
	// indent to make a hierarchy layout
	for (unsigned ws = 0; ws < n; ++ws) cout << ' ';
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
	if (cluster.left.get())
		printclust(*cluster.left, labels, n + 1);
	if (cluster.right.get())
		printclust(*cluster.right, labels, n + 1);
}

auto Bicluster::kcluster(const vector<vector<double>>& rows,
	/*function<double(const vector<double>&, const vector<double>&)> simularity,*/
	unsigned k
	)
{
	// Determine the minimum and maximum values for each point
	vector<pair<double, double>> ranges;
	auto rowsT = transpose(rows);
	for (const auto& row : rowsT)
	{
		auto minmax = minmax_element(row.cbegin(), row.cend());
		ranges.emplace_back(*minmax.first, *minmax.second);
	}

	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(0.0, 1.0);
	double rand = 0.0;
	vector<vector<unsigned>> bestmatches(k, { 0 }), lastmatches;
	vector<vector<double>> clusters;

	for (auto j : Util::range(k))
	{
		vector<double> cluster;
		for (auto i : Util::range(rows[0].size()))
		{
			rand = distribution(generator);
			auto number = rand*(ranges[i].second - ranges[i].first) + ranges[i].first;
			cluster.emplace_back(number);
		}
		clusters.emplace_back(cluster);
	}

	for (auto t : Util::range(100))
	{
		cout << "Iteration " << t << endl;

		unsigned bestmatch{ 0 };
		// Find which centroid is the closest for each row
		unsigned r = 0;
		for (const auto& row : rows)
		{
			bestmatch = 0;
			for (auto i : Util::range(k))
			{
				auto d = distance(clusters[i], row);
				if (d < distance(clusters[bestmatch], row))
					bestmatch = i;
			}
			bestmatches[bestmatch].push_back(r);
			++r;
		}

		// If the results are the same as last time, this is complete
		if (bestmatches == lastmatches)
			break;
		lastmatches = bestmatches;

		// Move the centroids to the average of their members
		for (auto i : Util::range(k))
		{
			vector<double> avgs(rows[0].size(), 0);
			if (bestmatches[i].size() > 0)
			{
				for (auto rowid : bestmatches[i])
					for (auto m : Util::range(rows[rowid].size()))
						avgs[m] += rows[rowid][m];
				for (auto j : Util::range(avgs.size()))
					avgs[j] /= bestmatches[i].size();
				clusters[i] = avgs;
			}
		}
	}
	return bestmatches;
}

auto Bicluster::Func(const Util::Similarity& sim)
{
	if (sim == Util::Similarity::sim_distance)
	{
		return bind(&Bicluster::distance, *this, placeholders::_1, placeholders::_2);
	}
	if (sim == Util::Similarity::sim_pearson)
	{
		return bind(&Bicluster::cluster_pearson, *this, placeholders::_1, placeholders::_2);
	}
}

auto Bicluster::kclusterwrapper(const python::list& data, const Util::Similarity& sim, int k)
{
	vector<vector<double>> ldata;
	python::stl_input_iterator<python::list> begin(data), end;
	vector<python::list> li = vector<python::list>(begin, end);
	transform(li.begin(), li.end(), inserter(ldata, ldata.end()), [&](auto& e)
	{
		python::stl_input_iterator<double> begin(e), end;
		vector<double> li2 = vector<double>(begin, end);
		return li2;
	});
	kcluster(ldata, /*Bicluster::Func(sim),*/ k);

	return 1;
}
BOOST_PYTHON_MODULE(cluster)
{
	python::class_<Bicluster>("Bicluster")
		.def("cluster_pearson", &Bicluster::cluster_pearson)
		.def("hcluster", &Bicluster::hcluster)
		.def("kcluster", &Bicluster::kclusterwrapper)
		.def("printclust", &Bicluster::printclust)
		;
	python::enum_<Util::Similarity>("Similarity")
		.value("distance", Util::Similarity::sim_distance)
		.value("sim_pearson", Util::Similarity::sim_pearson)
		;
}

//int main()
//{
//	const string& path("c:\\work\\machinelearning2");
//	vector<string> colnames, rownames;
//	vector<vector<double>> data;
//	readfile(path, colnames, rownames, data);
//	Bicluster cluster;
//	function<double(const vector<double>&,
//		const vector<double>&)> similarity = bind(&Bicluster::cluster_pearson, ref(cluster), std::placeholders::_1, std::placeholders::_2);
//	//auto clusters = cluster.hcluster(data, similarity);
//	//clusters.printclust(clusters, rownames);
//	auto clust = cluster.kcluster(data, similarity);
//	for (const auto& r : Util::range(10))
//		for (const auto& j : clust[r])			
//			cout << rownames[clust[r][j]] << endl;
//		cout << endl;
//}