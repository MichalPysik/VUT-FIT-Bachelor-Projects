import sys
import math as m
import copy as c
#výsledky bez záruky
def distance(vector1,vector2):
    result = m.sqrt(m.pow(vector2[0]-vector1[0],2)+m.pow(vector2[1]-vector1[1],2)+m.pow(vector2[2]-vector1[2],2))
    print("sqrt( (" + str(vector2[0]) + " - " + str(vector1[0]) + ")^2 + (" + str(vector2[1]) + " - " + str(vector1[1]) + ")^2 + (" + str(vector2[2]) + " - " + str(vector1[2]) + ")^2 ) = " + str(result) )
    return result

def prumer(array,prev):
    vector=[0,0,0]
    for item in array:
        i=0
        for titem in item:
            vector[i]+=titem
            i+=1
    if len(array)!=0:
        i=0
        for item in vector:
            vector[i]=item/len(array)
            i+=1
        return vector
    else:
        return prev


p=[ [-2, 4, 3],[0, -4, 3],[-3, 0, 5]]     #zde zadat vaši variantu
w=[[ 0,-1,-2],[-3,-1,-3],[ 1,-3, 2],[-2,-2, 2],[ 1, 2,-4],[ 0,-4, 3],[ 1, 0,-3],[-3, 0, 0],[-2, 2,-4],[-2, 4, 3],[ 3,-2, 4],[ 2,-5,-4]]


deepCopy=c.deepcopy(p)
while True:
    g=[[],[],[]]    #nová množina shluků
    print("\ntěžiště:", p)
    for item in w:
        print("Bod: ",item)
        dist0=distance(p[0],item)
        dist1=distance(p[1],item)
        dist2=distance(p[2],item)
        print("vzdálenosti:",dist0,dist1,dist2)
        if dist0<=dist1 and dist0<=dist2:
            g[0].append(item)
        elif dist1<=dist0 and dist1<=dist2:
            g[1].append(item)
        elif dist2<=dist0 and dist2<=dist1:
            g[2].append(item)
        else:
            print("chyba")
            exit(1)
    print("skupina 1: ",g[0])
    print("skupina 2: ",g[1])
    print("skupina 3: ",g[2])
    p[0]=prumer(g[0],p[0])
    p[1]=prumer(g[1],p[1])
    p[2]=prumer(g[2],p[2])
    if p==deepCopy:
        break
    else:
        deepCopy=c.deepcopy(p)  #pokud se rozhodovací sady schodují po iteraci, algoritmus končí
    print("")
print("\n------------------------------------------------------------------------------------------------------\n")
print("výsledek: ")
print("středy: ",p)
print("skupina 1: ",g[0])
print("skupina 2: ",g[1])
print("skupina 3: ",g[2])
