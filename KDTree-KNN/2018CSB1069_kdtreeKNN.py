from asyncio.windows_events import NULL
import random
from time import time

class RootNode:
    def __init__(self, region):
        self.left=NULL
        self.right=NULL
        self.region=region

class InternalNode: 
    def __init__(self): 
        self.left = NULL
        self.right = NULL
        self.axis=-1
        self.val=-1
        self.parent=NULL
        self.childtype=" "

class LeafNode:
    def __init__(self, pts):
        self.points = pts
        self.parent=NULL
        self.left=NULL
        self.right=NULL
        self.childtype=" "


dataset = list(list())  # {id, x, y}

def generateDatset(N, coordinatesRange) :
    pointID = 1
    while(pointID != N+1) :
        XCoor =  random.randint(0, coordinatesRange)
        YCoor = random.randint(0, coordinatesRange)
        dataset.append([pointID, XCoor, YCoor])
        pointID = pointID + 1


def load_points(filename):
  fil=open(filename,"r+")
  n=fil.readlines()
  points=[]
  for i in n:
    points.append(list(map(int,i.split())))
  fil.close()
  return points

def median_index(point_set, idx):  # idx = 1 -> X=c   idx = 2 -> Y=c
    sort_list = sorted(point_set, key = lambda x: x[idx]) 
    l=len(sort_list)
    if l%2==1:
        k=l//2+1
    else:
        k=l//2
    while k<l and sort_list[k-1][idx]==sort_list[k][idx]: 
        k=k+1
    if k==l:
        k=l-1
        while k>1 and sort_list[k-1][idx]==sort_list[k][idx]:
            k=k-1
    return sort_list[:k], sort_list[k:], sort_list[k-1][idx]

def region(point_set, dim):
    mn=[]
    mx=[]
    for i in (1, dim):
        mn.append(min(x[i] for x in point_set))
        mx.append(max(x[i] for x in point_set))
    return [mn,mx]

def stretch(point_set, dim):
    mx=0
    d=0
    for i in (1, dim):
        xmax=max(x[i] for x in point_set)
        xmin=min(x[i] for x in point_set)
        if xmax-xmin>mx:
            mx=xmax-xmin
            d=i
    return d

def make_tree(tree, point_set, dim, alpha):
  #print (point_set)
  axis = stretch(point_set,dim)
  a,b,c = median_index(point_set,axis)
  tree.axis = axis
  tree.val = c
  #print (a,"----",b,"\n")
  if len(a)<=alpha:
      P=LeafNode(a)
      P.childtype="L"
      tree.left=P
      tree.left.parent=tree
  else:
    A=InternalNode()
    A.childtype="L"
    tree.left=A
    make_tree(A,a,dim,alpha)
    tree.left.parent=tree
  if len(b)<=alpha:
      Q=LeafNode(b)
      Q.childtype="R"
      tree.right=Q
      tree.right.parent=tree
  else:
      B=InternalNode()
      B.childtype="R"
      tree.right=B
      make_tree(B,b,dim,alpha)
      tree.right.parent=tree

def Visualize(tree):
    if(tree==NULL):
        return
    queue = []
    queue.append(tree)
    level = 0
    while queue:
        sz = len(queue)
        print("Level -",level)
        for _ in range(sz):
            curr = queue.pop(0)
            if curr.left:
                queue.append(curr.left)
            if curr.right:
                queue.append(curr.right)

            if(not isinstance(curr, LeafNode)):
                if curr.axis==1 :
                    if(isinstance(curr, RootNode)):
                        print("Root Node -- Dividing Axis Line is : X = ", curr.val, "Region : ", curr.region)
                    else : print(curr.childtype ,"-Internal Node -- Dividing Axis Line is : X = ", curr.val)
                else :
                    if(isinstance(curr, RootNode)):
                        print("Root Node -- Dividing Axis Line is : Y = ", curr.val, "Region : ", curr.region)
                    else : print(curr.childtype ,"-Internal Node -- Dividing Axis Line is : Y = ", curr.val)
            else:
                print(curr.childtype,"-Leaf Node -- Data Points : ", curr.points)

        level = level + 1
        print()

import heapq
def naiveKNN(dataset, x, y, k) :
    maxheap = [] # store {distance, point} pair in maxheap
    result = [[]]
    res = []

    for point in dataset:
        distance = calcuateDistance(point[1], point[2], x, y)*-1
        heapq.heappush(maxheap, [distance, point])
        if len(maxheap) > k:
            heapq.heappop(maxheap)

    while len(maxheap) > 0:
        popped = heapq.heappop(maxheap)
        result.append([popped[1][0],popped[0]])
        res.append([popped[1],popped[0]])

    return result, res

import numpy as np
def minDistance(x , y, x_min, x_max, y_min , y_max) :
    f1 = 0
    f2 = 0
    if(x<= x_max and x >= x_min) :
        f1=1
    else :
        f1=0
    if(y<= y_max and y >= y_min):
        f2=1
    else :
        f2=0
    if(f1*f2 ==1) :
        return 0
    if( f1==1 and f2==0):
        return min(abs(y-y_min),abs(y-y_max))
    if(f1==0 and f2==1) :
        return min(abs(x-x_min),abs(x-x_max))
    else :
        x1=min(abs(x-x_min),abs(x-x_max))
        y1=min(abs(y-y_min),abs(y-y_max))
        res= np.math.sqrt((x1*x1) + (y1*y1))
        return res


def calcuateDistance(x1,y1,x2,y2):
    return np.math.sqrt(((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)))

def preprocess(tree, pq, xmin, xmax, ymin, ymax, x, y, K, mp):
    if(len(pq)==K):
        return
    if(tree==NULL):
        return
    if(isinstance(tree, LeafNode)):
        pts = tree.points
        for pt in pts:
            dist = calcuateDistance(x,y,pt[1], pt[2])*-1
            if(len(pq) < K):
                heapq.heappush(pq, [dist, pt[0]])
                mp[pt[0]] = 1
            else :
                if(pq[0][0]*-1 > dist*-1):
                    popped = heapq.heappop(pq)
                    mp[popped[1]] = 0
                    heapq.heappush(pq, [dist, pt[0]])
                    mp[pt[0]] = 1
        return
    if(tree.axis == 1):
        flag = 0
        if(x <= min(xmax, tree.val)):
            flag = 1
            preprocess(tree.left, pq, xmin, min(xmax, tree.val), ymin, ymax, x, y, K, mp)
        else :
            flag = 2
            preprocess(tree.right, pq, max(xmin, tree.val), xmax, ymin, ymax, x, y, K, mp)
        if(len(pq) < K) :
            if(flag==1):
                preprocess(tree.right, pq, max(xmin, tree.val), xmax, ymin, ymax, x, y, K, mp)
            elif(flag==2):
                preprocess(tree.left, pq, xmin, min(xmax, tree.val), ymin, ymax, x, y, K, mp)
        return
    else :
        flag = 0
        if(y <= min(ymax, tree.val)):
            flag = 1
            preprocess(tree.left, pq, xmin, xmax, ymin, min(ymax, tree.val), x, y, K, mp)
        else :
            flag = 2
            preprocess(tree.right, pq, xmin, xmax, max(ymin, tree.val), ymax, x, y, K, mp)
        if(len(pq) < K):
            if(flag==1):
                preprocess(tree.right, pq, xmin, xmax, ymin, min(ymax, tree.val), x, y, K, mp)
            elif(flag==2):
                preprocess(tree.left, pq, xmin, xmax, max(ymin, tree.val), ymax, x, y, K, mp)
        return


def treeTraversal(tree, pq, xmin, xmax, ymin, ymax, x, y, K, mp):
    if(tree==NULL):
        return
    if(isinstance(tree, LeafNode)):
        pts = tree.points
        for pt in pts:
            dist = calcuateDistance(x,y,pt[1], pt[2]) * -1
            if((pq[0][0]*-1 > dist*-1) and (mp[pt[0]]==0)):
                popped = heapq.heappop(pq)
                mp[popped[1]] = 0
                heapq.heappush(pq, [dist, pt[0]])
                mp[pt[0]] = 1
        return
    if(tree.axis == 1):
        dist=minDistance(x,y,xmin,min(xmax, tree.val),ymin,ymax)*-1
        if(pq[0][0]*-1 > dist*-1):
            treeTraversal(tree.left, pq, xmin, min(xmax, tree.val), ymin, ymax, x, y, K, mp)
        dist = minDistance(x,y,max(xmin, tree.val),xmax,ymin,ymax)*-1
        if(pq[0][0]*-1 > dist*-1) :
            treeTraversal(tree.right, pq, max(xmin, tree.val), xmax, ymin, ymax, x, y, K, mp)

        return
    else :
        dist=minDistance(x,y,xmin,xmax,ymin,min(ymax, tree.val))*-1
        if(pq[0][0]*-1 > dist*-1):
            treeTraversal(tree.left, pq, xmin, xmax, ymin, min(ymax, tree.val), x, y, K, mp)
        dist = minDistance(x,y,max(xmin, tree.val),xmax,ymin,ymax)*-1
        if(pq[0][0]*-1 > dist*-1) :
            treeTraversal(tree.right, pq, xmin, xmax, max(ymin, tree.val), ymax, x, y, K, mp)
        return
    
def experiment(alpha, k_list, queries, file):
    global dataset
    execution_Time_naive = []
    execution_Time_kdtree = []
    dataset = load_points(file)
    dimension = len(dataset[0])-1
    Reg = region(dataset, dimension)
    kd_tree = RootNode(Reg)
    make_tree(kd_tree, dataset, dimension, alpha)
    
    # queryPoints = [[1,2], [6,12], [2,7], [23,13],[23,400],[29,200]]

    queryPoints = list(list())
    for i in range(queries):
        XCoor =  random.randint(0, 400)
        YCoor = random.randint(0, 400)
        queryPoints.append([XCoor, YCoor])

    for K in k_list:
        time_naive = []
        time_kdtree = []
        for pt in queryPoints:
            pq = []
            mp = {}

            for dt in dataset:
                mp[dt[0]] = 0

            xmin = Reg[0][0]
            ymin = Reg[0][1]
            xmax = Reg[1][0]
            ymax = Reg[1][1]
            start_clock = time() # naive algo started
            res = naiveKNN(dataset, pt[0], pt[1], K)
            finish_clock = time()
            time_naive.append(finish_clock-start_clock)
            
            start_clock = time() # kdtree knn algo started
            preprocess(kd_tree, pq, xmin, xmax, ymin, ymax, pt[0], pt[1], K, mp)
            treeTraversal(kd_tree, pq, xmin, xmax, ymin, ymax, pt[0], pt[1], K, mp)
            finish_clock = time()
            time_kdtree.append(finish_clock-start_clock)
        execution_Time_naive.append(sum(time_naive) / len(time_naive))
        execution_Time_kdtree.append(sum(time_kdtree) / len(time_kdtree))
    
    return execution_Time_naive, execution_Time_kdtree

def main():
    global dataset
    # generateDatset(10, 5)
    # dataset = [[1, 3, 6], [2, 17, 15], [3, 13, 15], [4, 6, 12], [5, 9, 1], [6, 2, 7], [7, 10, 19]]
    dataset = load_points("nodes.txt")

    # print(median_index(dataset, 2))
    
    dimension = len(dataset[0])-1
    alpha=int(input("Enter alpha : "))
    Reg = region(dataset, dimension)
    kd_tree = RootNode(Reg)
    make_tree(kd_tree, dataset, dimension, alpha)
    
    # Visualize(kd_tree)

    queryPoints = [[100,100],[200,200],[300,300],[400,400]]
    print("Reg ",Reg[0][0], Reg[0][1], Reg[1][0], Reg[1][1])
    K = int(input("Enter K : "))
    for pt in queryPoints:
        print()
        pq = []
        mp = {}

        for dt in dataset:
            mp[dt[0]] = 0

        xmin = Reg[0][0]
        ymin = Reg[0][1]
        xmax = Reg[1][0]
        ymax = Reg[1][1]
        start_clock = time() # naive algo started
        res = naiveKNN(dataset, pt[0], pt[1], K)
        finish_clock = time()
        print("Time Taken By Naive: " + "%.4f" % (finish_clock - start_clock) + " seconds")
        print("Query point is ",pt," ", K,"-NN By NAIVE ALGO: ", res[1])
        
        start_clock = time() # kdtree knn algo started
        preprocess(kd_tree, pq, xmin, xmax, ymin, ymax, pt[0], pt[1], K, mp)
        treeTraversal(kd_tree, pq, xmin, xmax, ymin, ymax, pt[0], pt[1], K, mp)
        finish_clock = time()
        print("Time Taken By KDTree: " + "%.4f" % (finish_clock - start_clock) + " seconds")
        KdtreeknnAns = pq
        print("Query point is ",pt," ", K,"-NN By KDTREE: ", KdtreeknnAns)

    print()
    execution_Time_naive, execution_Time_kdtree =  experiment(30, [5,20,50,100], 20, "dataset.txt")
    print("Experiment 1: Naive -->", execution_Time_naive)
    print("KDTre -->", execution_Time_kdtree)

    execution_Time_naive, execution_Time_kdtree = experiment(100, [5,20,50,100], 20, "dataset.txt")
    print("Experiment 2: Naive -->", execution_Time_naive)
    print("KDTre -->", execution_Time_kdtree)


if __name__== "__main__":
  main() 