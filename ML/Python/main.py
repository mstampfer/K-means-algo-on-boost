import sys
import os
print(os.getcwd())
import sys
print(sys.path)
import ml_ext

planet = ml_ext.World('Hi!')
xx = planet.greet()
print(xx)
