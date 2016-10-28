# encoding=utf8  
"""
Created on Sat Oct 15 16:09:04 2016
"""

import sys as sys
import math as math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import cm as cm
import scipy.linalg as la


L = 5.0
a = 0.0
tf = 2.0

f = lambda x: 1.0 if 2 < x < 4 else 0.0
h = lambda x: 0.5
g = lambda x: 0.5

alpha = 1.0
ro = 0.1
beta = 1.0
sigma = 0.1

p = 1
q = 0
r = 0

Nx = 25
Nt = 30


dx = L/Nx
dt = tf/Nt
fn = lambda n: f(a + n*dx)
Nx1 = Nx + 1

if abs(alpha*dx - ro) < 1e-7:
    sys.exit('alpha*dx - ro == 0: {0}*{1} - {2}'.format(alpha, dx, ro))



T = []
T0 = []
T.append(T0)
T0.append((dx*h(0) - ro*fn(1))/(alpha*dx - ro))
for n in xrange(1, Nx1):
    T0.append(fn(n))
T0.append((dx*g(0) + sigma*fn(Nx))/(beta*dx + sigma))

vmin = min(T0)
vmax = max(T0)

def draw(data, cmap):
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    cax = ax1.imshow(data, interpolation="nearest", cmap=cmap)
    fig.colorbar(cax, shrink=0.75, aspect=5)
    ax1.grid(True)
    plt.show()

print T0
draw(np.matrix([T0]), 'hot')

def TT(k,i):
    return T[i][k]

# montando a matriz que representa o sistema a ser resolvido
# essa matriz não varia com o tempo
lines = []
for lin in xrange(1, Nx1):
    cols = []
    for col in xrange(1, Nx1):
        val = 0.0
        
        if (col == lin - 1):
            val = -p*dt
        elif (col == lin):
            val = 2*dx*dx + 2*p*dt + q*dx*dt - r*dx*dx*dt
        elif (col == lin + 1):
            val = -dt*(p + q*dx)
        
        if (lin == 1 and col == 1):
            val = val + (ro*p*dx)/(alpha*dx - ro)
        elif (lin == Nx and col == Nx):
            val = val - sigma*dt*(p + q*dx)/(beta*dx + sigma)
        
        cols.append(val)
    lines.append(cols)
matA = np.matrix(lines)

print matA.shape
draw(matA, cm.coolwarm)

# montando o vetor resultado do sistema
# esse vetor varia com o tempo, e deve ser remontado a cada iteração de tempo
def calcNextTimeResultVec(i):
    result = []
    for k in xrange(1, Nx1):
        res = TT(k + 1, i)*dt*(p + q*dx) - TT(k, i)*(-2*dx*dx + 2*p*dt + q*dx*dt - r*dx*dx*dt) + TT(k - 1, i)*p*dt
        if (k == 1):
            res = res - (dx*h((i + 1)*dt))/(alpha*dx - ro)
        elif (k == Nx):
            res = res - (dx*g((i + 1)*dt))/(beta*dx + sigma)
        result.append([res])
    return np.matrix(result)

def drawVec(data, cmap):
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    ax1.imshow(data, interpolation="nearest", cmap=cmap, vmin=vmin, vmax=vmax)
    plt.show()

drawVec(np.matrix([T0]), 'hot')
# resolvendo iterativamente cada passo no tempo
for t in xrange(1, Nt):
    vecB = calcNextTimeResultVec(t - 1)
    Ti = la.solve(matA, vecB)
    Ti = list(Ti.flat)
    #print Ti
    Ti[:0] = [(dx*h(t*dt) - ro*Ti[0])/(alpha*dx - ro)]
    Ti.append((dx*g(t*dt) + sigma*Ti[-1])/(beta*dx + sigma))
    drawVec(np.matrix([Ti]), 'hot')
    T.append(Ti)
