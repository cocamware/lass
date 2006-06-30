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

def patch(xs,ys,s1='Color',s2='k'):
    if operator.isSequenceType(xs):
        if s1=='Color':
            pylab.fill(xs,ys,s2)
        else:
            pylab.fill(xs,ys,'k')
    else:
        pylab.plot([xs],[ys],'bo')
    return None


def set(x,y,z):
    pass

for i in range(1,len(sys.argv)):
    execfile(sys.argv[i])

pylab.show()
        
