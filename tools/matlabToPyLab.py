import sys
import pylab
import operator

def line(xs,ys,s1='Color',s2='k'):
    if operator.isSequenceType(xs):
        if s1=='Color':
            pylab.plot(xs,ys,s2)
        else:
            pylab.plot(xs,ys,'k')
    else:
        pylab.plot([xs],[ys],'bo')
    return None

def set(x,y,z):
    pass

execfile(sys.argv[1])

pylab.show()
        
