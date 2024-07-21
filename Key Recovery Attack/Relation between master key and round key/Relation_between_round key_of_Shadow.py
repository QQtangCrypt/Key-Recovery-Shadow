"""
This code is used to test the relation between round key of SHADOW
Data: 2024 01 23
K[64] is the index of key
C[64] is used to record the number of adding constant
N[64] is used to record the number of NX modular
"""
K=[i for i in range(64)]
C=[0 for i in range(64)]
N=[0 for i in range(64)]
Per=[56,57,58,59,16,17,18,19,20,21,22,23,24,25,26,27,60,61,62,63,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
Perinv=[48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,4,5,6,7,8,9,10,11,12,13,14,15,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,0,1,2,3,16,17,18,19]

def Permutation(P):
    temp=[0 for i in range(64)]
    for i in range(64):
        temp[i]=P[Per[i]]
    return temp
def Permutationinv(P):
    temp=[0 for i in range(64)]
    for i in range(64):
        temp[i]=P[Perinv[i]]
    return temp
def Constant(X):
    X[3]=X[3]+1
    X[4]=X[4]+1
    X[5]=X[5]+1
    X[6]=X[6]+1
    X[7]=X[7]+1
    return X
def NX(X):
    X[56]=X[56]+1
    X[57]=X[57]+1
    X[58]=X[58]+1
    X[59]=X[59]+1
    X[60]=X[60]+1
    X[61]=X[61]+1
    X[62]=X[62]+1
    X[63]=X[63]+1
    return X

def Constantinv(X):
    X[3]=X[3]-1
    X[4]=X[4]-1
    X[5]=X[5]-1
    X[6]=X[6]-1
    X[7]=X[7]-1
    return X
def NXinv(X):
    X[56]=X[56]-1
    X[57]=X[57]-1
    X[58]=X[58]-1
    X[59]=X[59]-1
    X[60]=X[60]-1
    X[61]=X[61]-1
    X[62]=X[62]-1
    X[63]=X[63]-1
    return X


def Output(X):
    for i in range(8):
        temp=[]
        for j in range(8):
            temp.append(X[i*8+j])
        print(temp)

def Output0(X):
    temp=[0 for i in range(8)]
    temp[0]=X[0]
    temp[1] = X[1]
    temp[2] = X[2]
    temp[3] = X[3]
    temp[4] = X[8]
    temp[5] = X[9]
    temp[6] = X[10]
    temp[7] = X[11]
    return temp
def Output1(X):
    temp=[0 for i in range(8)]
    temp[0]=X[4]
    temp[1] = X[5]
    temp[2] = X[6]
    temp[3] = X[7]
    temp[4] = X[12]
    temp[5] = X[13]
    temp[6] = X[14]
    temp[7] = X[15]
    return temp

def Output2(X):
    temp=[0 for i in range(8)]
    temp[0]=X[16]
    temp[1] = X[17]
    temp[2] = X[18]
    temp[3] = X[19]
    temp[4] = X[24]
    temp[5] = X[25]
    temp[6] = X[26]
    temp[7] = X[27]
    return temp

def Output3(X):
    temp=[0 for i in range(8)]
    temp[0]=X[20]
    temp[1] = X[21]
    temp[2] = X[22]
    temp[3] = X[23]
    temp[4] = X[28]
    temp[5] = X[29]
    temp[6] = X[30]
    temp[7] = X[31]
    return temp


"""
print("********************************************")
print("The backward round")
print("********************************************")
for r in range(1):
    print("============================================================")
    print("The round key of the ", r,"round")

    K=Permutationinv(K)
    C=Permutationinv(C)
    N=Permutationinv(N)
    C=Constantinv(C)
    N=NXinv(N)
    print("The order of K:")
    Output(K)
    print("--------------------------------------------------------")
    print("The Overlap of C and N after ",r," round")
    temp = [0 for i in range(64)]
    for i in range(64):
        temp[i] = C[i] + N[i]
    Output(temp)
    print("--------------------------------------------------------")
    print("The record of add constant:")
    Output(C)
    print("The record of NX:")
    Output(N)
"""
F=[0 for i in range(64)]
print("********************************************")
print("The forward round")
print("********************************************")
K=[i for i in range(64)]
print(Output(K))
C=[0 for i in range(64)]
N=[0 for i in range(64)]
for r in range(16):
    print("============================================================")
    print("The round key of the ", r,"round")
    C=Constant(C)
    N=NX(N)
    K=Permutation(K)
    C=Permutation(C)
    N=Permutation(N)
    for k in range(32):
        F[K[k]]=F[K[k]]+1
    print("The order of K:")
    Output(K)
    print("--------------------------------------------------------")
    print("The Overlap of C and N after ",r," round")
    temp = [0 for i in range(64)]
    for i in range(64):
        temp[i] = C[i] + N[i]
    Output(temp)
    print("--------------------------------------------------------")
    print("The record of add constant:")
    Output(C)
    print("The record of NX:")
    Output(N)

print(F)