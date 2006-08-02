import sys
import pylab
import operator

c = (250,250)
w = 250

box_min = (c[0]-w, c[1]-w)
box_max = (c[0]+w, c[1]+w)

def clip(xs,ys):
    for i in range(len(xs)):
        if xs[i]>box_min[0] and xs[i]<box_max[0] and ys[i]>box_min[1] and ys[i]<box_max[1]:
            return (xs,ys)
    return ([],[])

def clipSingle(xs,ys):
    if xs>box_min[0] and xs<box_max[0] and ys>box_min[1] and ys<box_max[1]:
        return (xs,ys)
    return None
    

def line(xs,ys,s1='Color',s2='k'):
    if operator.isSequenceType(xs):
        (nxs,nys) = clip(xs,ys)
        if len(nxs)>0:
            if s1=='Color':
                pylab.plot(xs,ys,s2)
            else:
                pylab.plot(xs,ys,'k')
    else:
        cp = clipSingle(xs,ys)
        if cp!=None:
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

def buildFilteredTemp(iFile):
    f = open(iFile,'r')
    fo= open('temp.m','w')
    l = f.readline()
    c = 0
    while len(l)>0:
        try:
            exec(l)
        except:
            print '\nerror in :'
            print l
        l = f.readline()
        c+=1
        if c%1000==0:
            print c, 'lines processed\r',
    fo.close()
    f.close()
    return 'temp.m'

for i in range(1,len(sys.argv)):
    filename = buildFilteredTemp(sys.argv[i])
    #execfile(filename)
 

pylab.show()
        
