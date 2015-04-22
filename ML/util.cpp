//Returns the Pearson correlation coefficient for p1 and p2
#include "util.h"


double Util::distance(const vector<pair<double, double>>& common)
{
	auto sum_of_squares = 0.0;

	for_each(common.cbegin(), common.cend(), [&sum_of_squares](const auto& item) {
		sum_of_squares += pow(item.first - item.second, 2);
	});
	if (sum_of_squares == 0)
		return 0.0;
	return 1.0 / (1.0 + sum_of_squares);
}

double Util::pearson(const vector<pair<double,double>>& common)
{
	//       Add up all the preferences
	auto sum1 = 0.0, sum2 = 0.0;
	for_each(common.cbegin(), common.cend(), [&sum1, &sum2](const auto& item) {
		sum1 += item.first;
		sum2 += item.second;
	});

	//       Sum up the squares
	auto sum1Sq = 0.0, sum2Sq = 0.0;
	for_each(common.cbegin(), common.cend(), [&sum1Sq, &sum2Sq](const auto& item) {
		sum1Sq += pow(item.first, 2);
		sum2Sq += pow(item.second, 2);
	});

	//       Sum up the products
	auto pSum = 0.0;
	for_each(common.cbegin(), common.cend(), [&pSum](const auto& item) {
		pSum += item.first*item.second;
	});

	//       Calculate Pearson score
	const auto& n = common.size();
	const auto& num = pSum - (sum1*sum2 / n);
	const auto& den = sqrt((sum1Sq - pow(sum1, 2) / n)*(sum2Sq - pow(sum2, 2) / n));
	if (den == 0) return 0.0;
	const auto& r = num / den;
	return r;
}

vector<unsigned> Util::range(unsigned len, unsigned start) {
	vector<unsigned> v(len);
	iota(v.begin(), v.end(), start);
	return v;
};