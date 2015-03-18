#include <map>
#include <string>
#include <iostream>

using namespace std;

auto sim_distance(const map<const string, map<const string, float>>& prefs,
                  const string& person1,
                  const string& person2)
{
// Get the list of shared_items
//   for(const auto & item1 : prefs.at(person1) ) {
//      for (const auto &item2 : prefs.at(person2)) {
//         if (item1.first == item2.first)
//            si[item1.first] = 1.0;
//      }
//   }
    map<string, float> si;
    auto person2_items = prefs.at(person2);
    for(const auto & person1_item : prefs.at(person1) ) {
        for_each(person2_items.cbegin(), person2_items.cend(), [&person1_item, &si](auto &person2_item) {
            if (person1_item.first == person2_item.first)
                si[person1_item.first] = 1.0;
        });
    }



   // if they have no ratings in common, return 0
      if (si.size()==0)
         return 0.0;
    else
          cout << si.size();

   // Add up the squares of all the differences
      auto sum_of_squares=0;//sum([pow(prefs[person1][item]-prefs[person2][item],2)
                          //for item in prefs[person1] if item in prefs[person2]])
      return 1.0/(1.0+sum_of_squares);
}

void main() {


   const map<const string, map<const string, float>> critics = {
           {"Lisa Rose", {{"Lady in the Water", 2.5}, {"Snakes on a Plane", 3.5},
                   {"Just My Luck", 3.0}, {"Superman Returns", 3.5}, {"You, Me and Dupree", 2.5},
                   {"The Night Listener", 3.0}}},
           {"Gene Seymour", {{"Lady in the Water", 3.0}, {"Snakes on a Plane", 3.5},
                   {"Just My Luck", 1.5}, {"Superman Returns", 5.0}, {"The Night Listener", 3.0},
                   {"You, Me and Dupree", 3.5}}},
           {"Michael Phillips", {{"Lady in the Water", 2.5}, {"Snakes on a Plane", 3.0},
                   {"Superman Returns", 3.5}, {"The Night Listener", 4.0}}},
           {"Claudia Puig", {{"Snakes on a Plane", 3.5}, {"Just My Luck", 3.0},
                   {"The Night Listener", 4.5}, {"Superman Returns", 4.0},
                   {"You, Me and Dupree", 2.5}}},
           {"Mick LaSalle", {{"Lady in the Water", 3.0}, {"Snakes on a Plane", 4.0},
                   {"Just My Luck", 2.0}, {"Superman Returns", 3.0}, {"The Night Listener", 3.0},
                   {"You, Me and Dupree", 2.0}}},
           {"Jack Matthews", {{"Lady in the Water", 3.0}, {"Snakes on a Plane", 4.0},
                   {"The Night Listener", 3.0}, {"Superman Returns", 5.0}, {"You, Me and Dupree", 3.5}}},
           {"Toby", {{"Snakes on a Plane", 4.5}, {"You, Me and Dupree", 1.0}, {"Superman Returns", 4.0}}}
   };
    sim_distance(critics, "Lisa Rose", "Gene Seymour");
}


