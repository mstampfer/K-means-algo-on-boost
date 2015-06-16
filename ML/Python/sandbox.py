import playground
import os



critics={
	'Lisa Rose': {'Lady in the Water': 2.5}, 
	'Lisa Rose': {'Snakes on a Plane': 3.5},
	'Lisa Rose': {'Just My Luck': 3.0}, 
	'Lisa Rose': {'Superman Returns': 3.5}, 
	'Lisa Rose': {'You, Me and Dupree': 2.5},
	'Lisa Rose': {'The Night Listener': 3.0},
    'Gene Seymour': {'Lady in the Water': 3.0}, 
	'Gene Seymour': {'Snakes on a Plane': 3.5},
	'Gene Seymour': {'Just My Luck': 1.5}, 
	'Gene Seymour': {'Superman Returns': 5.0}, 
	'Gene Seymour': {'The Night Listener': 3.0},
	'Gene Seymour': {'You, Me and Dupree': 3.5}
	}
#playground.myClass().SetParameters(critics)

#f = playground.myClass.choose_function(1)
#g = playground.myClass.choose_function(0)
#print(f())
#print(g())
#playground.myClass.sequence_to_int_list([1,2,3,4,5,7])

##print(playground.foo(1,1).__doc__
print(playground.identity(playground.color.red))

#print(playground.identity(playground.green))

print(playground.identity(playground.color.green))

#print(playground.identity(playground.blue))
print(playground.identity(playground.color.blue))

print(playground.identity(playground.color(1)))

print(playground.identity(playground.color(2)))

print(playground.identity(playground.color(3)))

print(playground.identity(playground.color(4)))

##playground.something_which_throws();


#print(playground.yes)
#y = playground.X.Y()
#print(y.g())
playground.myClass().testDict(critics)
