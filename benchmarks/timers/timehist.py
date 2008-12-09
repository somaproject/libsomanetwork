import pylab
import numpy as n

x = n.fromfile('time.dat', dtype=n.uint64)
print len(x)
print x[:10]
pylab.hist(x)

pylab.show()
