#include <map>
#include <string>
#include <iostream>
#include <math.h>
#include <vector>

template <typename T>
class TD;


using namespace std;
using PreferenceT = const pair<const string, const float>;
using PreferencePairT = pair<const string, PreferenceT>;
using PreferenceMMapT = const multimap<const string, PreferenceT>;
using CommonPrefMapT = multimap<string, pair<float,float>>;


auto find_common(PreferenceMMapT & prefs, const string& person1, const string& person2) {
    CommonPrefMapT common_items;
    const auto p1_items = prefs.equal_range(person1);
    const auto p2_items = prefs.equal_range(person2);
    vector<PreferencePairT> person1v(p1_items.first,p1_items.second);
    vector<PreferencePairT> person2v(p2_items.first,p2_items.second);

    for(auto e1 : person1v) {
        for(auto e2 : person2v) {
            auto e1pair = e1.second; auto e2pair = e2.second;
            if (e1pair.first == e2pair.first) {
                auto p = make_pair(e1pair.second, e2pair.second);
                common_items.insert(pair<string, decltype(p)>(e1.first, p));
            }
        }
    }
    return common_items;
}

auto sim_distance(PreferenceMMapT & prefs,
        const string& person1,
        const string& person2)
{
    auto sum_of_squares = 0.0;
    auto common = find_common(prefs, person1, person2);
    for_each(common.cbegin(),common.cend(),[&sum_of_squares](auto& item)
    {
        sum_of_squares += pow(item.second.first - item.second.second, 2);
    });
    if (sum_of_squares == 0)
        return 0.0;
    return 1.0/(1.0+sum_of_squares);
}


//Returns the Pearson correlation coefficient for p1 and p2
auto sim_pearson(const PreferenceMMapT & prefs, const string& person1, const string& person2) {

// Get the list of mutually rated items
    auto common = find_common(prefs, person1, person2);
    if (common.size() == 0.0)
        return 0.0;

//       Add up all the preferences
    auto sum1=0.0, sum2=0.0;
    for_each(common.cbegin(), common.cend(), [&sum1, &sum2](auto prefMap)
    {
        sum1 += prefMap.second.first;
        sum2 += prefMap.second.second;
    });

//       Sum up the squares
    auto sum1Sq=0.0, sum2Sq=0.0;
    for_each(common.cbegin(), common.cend(), [&sum1Sq, &sum2Sq](auto prefMap)
    {
        sum1Sq += pow(prefMap.second.first,2);
        sum2Sq += pow(prefMap.second.second,2);
    });

//       Sum up the products
    auto pSum = 0.0;
    for_each(common.cbegin(), common.cend(), [&pSum](auto prefMap)
    {
        pSum += prefMap.second.first*prefMap.second.second;
    });

//       Calculate Pearson score
        auto n = common.size();
        auto num=pSum-(sum1*sum2/n);
        auto den=sqrt((sum1Sq-pow(sum1,2)/n)*(sum2Sq-pow(sum2,2)/n));
        if (den==0) return 0.0;
        auto r=num/den;
        return r;
}

// Returns the best matches for person from the prefs dictionary.
// Number of results and similarity function are optional params.
auto topMatches(const PreferenceMMapT & prefs,
        const string& person)//,
//        function<double(
//                const PreferenceMMapT & prefs,
//                const string& person1,
//                const string& person2)> similarity=sim_pearson)
{
    std::map<double, string, greater<double> > scores;
    for_each(prefs.cbegin(), prefs.cend(), [&prefs, &scores](auto& pref){
        if (pref.first != "Toby")
            scores.insert(make_pair(sim_pearson(prefs, pref.first, "Toby"), pref.first));
    });
    return scores;
}

int main() {

   const PreferenceMMapT critics = {
           {"Lisa Rose", {"Lady in the Water", 2.5}},
           {"Lisa Rose", {"Snakes on a Plane", 3.5}},
           {"Lisa Rose", {"Just My Luck", 3.0}},
           {"Lisa Rose", {"Superman Returns", 3.5}},
           {"Lisa Rose", {"You, Me and Dupree", 2.5}},
           {"Lisa Rose", {"The Night Listener", 3.0}},
           {"Gene Seymour", {"Lady in the Water", 3.0}},
           {"Gene Seymour", {"Snakes on a Plane", 3.5}},
           {"Gene Seymour", {"Just My Luck", 1.5}},
           {"Gene Seymour", {"Superman Returns", 5.0}},
           {"Gene Seymour", {"The Night Listener", 3.0}},
           {"Gene Seymour", {"You, Me and Dupree", 3.5}},
           {"Michael Phillips", {"Lady in the Water", 2.5}},
           {"Michael Phillips", {"Snakes on a Plane", 3.0}},
           {"Michael Phillips", {"Superman Returns", 3.5}},
           {"Michael Phillips", {"The Night Listener", 4.0}},
           {"Claudia Puig", {"Snakes on a Plane", 3.5}},
           {"Claudia Puig", {"Just My Luck", 3.0}},
           {"Claudia Puig", {"The Night Listener", 4.5}},
           {"Claudia Puig", {"Superman Returns", 4.0}},
           {"Claudia Puig", {"You, Me and Dupree", 2.5}},
           {"Mick LaSalle", {"Lady in the Water", 3.0}},
           {"Mick LaSalle", {"Snakes on a Plane", 4.0}},
           {"Mick LaSalle", {"Just My Luck", 2.0}},
           {"Mick LaSalle", {"Superman Returns", 3.0}},
           {"Mick LaSalle", {"The Night Listener", 3.0}},
           {"Mick LaSalle", {"You, Me and Dupree", 2.0}},
           {"Jack Matthews", {"Lady in the Water", 3.0}},
           {"Jack Matthews", {"Snakes on a Plane", 4.0}},
           {"Jack Matthews", {"The Night Listener", 3.0}},
           {"Jack Matthews", {"Superman Returns", 5.0}},
           {"Jack Matthews", {"You, Me and Dupree", 3.5}},
           {"Toby", {"Snakes on a Plane", 4.5}},
           {"Toby", {"You, Me and Dupree", 1.0}},
           {"Toby", {"Superman Returns", 4.0}}};

    cout << sim_distance(critics, "Lisa Rose", "Gene Seymour") << endl;
    cout << sim_pearson(critics, "Lisa Rose", "Gene Seymour") << endl;
    auto scores = topMatches(critics,"Toby");
    for_each(scores.cbegin(), scores.cend(), [](auto& score){
        cout << score.first << ", " << score.second << endl;
    });
    return 0;
}


