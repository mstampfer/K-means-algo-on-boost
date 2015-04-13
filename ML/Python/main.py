import ml_ext


#a = ml_ext.A()
#a.b = [(1,2),(3,4),(5,6)]
##a.b = [1,2,3,4,5]
#print(a.b)

#a.op(a.b)

A = ml_ext.myClass();
a2 = {"a2":2.2}
d2 = {"d2":4.3}
b2 = {"a2":9.3}

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

A.SetParameters(critics, a = a2, d = d2, b = b2)

#planet = ml_ext.World('Hi!')
#print(planet.greet())

#critics = ml_ext.Recommendation.buildDict()
pass