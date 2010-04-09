import lass
import random

size = 1000

def sqrDistance(iA, iB):
    result = 0.0
    for k in range(2):
        result += (iA[k] - iB[k]) ** 2
    return result


def createTree(numberOfPoints = 100):
    # create a list of points
    #
    nSet = numberOfPoints
    aset = []
    for i in range(nSet):
        x = random.uniform(0, size)
        y = random.uniform(0, size)
        value = random.uniform(0, size)
        aset.append(lass.KdTree2D.Vertex2D((x, y), value))
    # create a tree
    #
    kdTree = lass.KdTree2D(aset)
    return (aset,kdTree)

def testNearest(aset,tree,numberOfTest = 10):
    nTests = numberOfTest
    for i in range(nTests):
        x = random.uniform(0, size)
        y = random.uniform(0, size)
        test = (x, y)
        
        # naive search
        naiveNearest = aset[0]
        naiveSqrDistance = sqrDistance(test, naiveNearest.position)
        for p in aset[1:]:
            sqrDist = sqrDistance(test, p.position)
            if sqrDist < naiveSqrDistance:
                naiveNearest = p
                naiveSqrDistance = sqrDist

        # tree search
        print "tree search"
        treeNearest = tree.nearestNeighbour(test)

        if 0:
            print "test point: ", test
            print "naive nearest: ", naiveNearest.position, naiveNearest.value
            print "KdTree nearest: ", treeNearest.position, treeNearest.value
            print ""
        if naiveNearest.value!=treeNearest.value:
            print "naive nearest and KdTree nearest did not produce same result"
            return 1
    return 0

(theSet,theTree) = createTree(100)
if testNearest(theSet,theTree,10):
    print "test failed"
    
