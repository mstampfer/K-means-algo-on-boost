import ml_ext
import pprint

critics={'Lisa Rose': {'Lady in the Water': 2.5, 
					   'Snakes on a Plane': 3.5,
					   'Just My Luck': 3.0, 
					   'Superman Returns': 3.5, 
					   'You, Me and Dupree': 2.5,
					   'The Night Listener': 3.0},
     'Gene Seymour': {'Lady in the Water': 3.0, 
					  'Snakes on a Plane': 3.5,
		  			  'Just My Luck': 1.5, 
					  'Superman Returns': 5.0, 
					  'The Night Listener': 3.0,
					  'You, Me and Dupree': 3.5},
     'Michael Phillips': {'Lady in the Water': 2.5, 
						  'Snakes on a Plane': 3.0,
						  'Superman Returns': 3.5, 
						  'The Night Listener': 4.0},
     'Claudia Puig': {'Snakes on a Plane': 3.5, 
					  'Just My Luck': 3.0,
					  'The Night Listener': 4.5, 
					  'Superman Returns': 4.0,
					  'You, Me and Dupree': 2.5},
     'Mick LaSalle': {'Lady in the Water': 3.0, 
					  'Snakes on a Plane': 4.0,
					  'Just My Luck': 2.0, 
					  'Superman Returns': 3.0, 
					  'The Night Listener': 3.0,
					  'You, Me and Dupree': 2.0},
     'Jack Matthews': {'Lady in the Water': 3.0, 
					   'Snakes on a Plane': 4.0,
					   'The Night Listener': 3.0, 
					   'Superman Returns': 5.0, 
					   'You, Me and Dupree': 3.5},
     'Toby': {'Snakes on a Plane':4.5,
			  'You, Me and Dupree':1.0,
			  'Superman Returns':4.0}
	 }
rec = ml_ext.Recommendation(critics)
rec.critics_p
print(rec.sim_distance('Lisa Rose','Gene Seymour'))
print(rec.sim_pearson('Lisa Rose','Gene Seymour'))
tm =rec.topMatches('Toby', ml_ext.Similarity.distance)
pprint.pprint(tm)
pass

rankings = rec.getRecommendations( "Toby", ml_ext.Similarity.distance)
pprint.pprint(rankings)
pass

rec.transformPrefs()
tm = rec.topMatches("Superman Returns", ml_ext.Similarity.distance)
pprint.pprint(tm)
pass

itemsIm = rec.calculateSimilarItems()
pprint.pprint(itemsIm)

recs = rec.getRecommendedItems2( itemsIm, "Toby" )
pprint.pprint(recs)

def loadMovieLens(path='/data/movielens'):
# Get movie titles
	movies={}
	for line in open(path+'/u.item'):
		(id,title)=line.split('|')[0:2]
		movies[id]=title
# Load data
	prefs={}
	for line in open(path+'/u.data'):
		(user,movieid,rating,ts)=line.split('\t')
 		prefs[user][movies[movieid]]=float(rating)
	return prefs

prefs 

