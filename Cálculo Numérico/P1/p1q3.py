# encoding=utf8  
"""
Created on Sat Oct 15 16:09:04 2016
"""

import sys as sys
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import cm as cm
from matplotlib.colors import LinearSegmentedColormap
import scipy.linalg as la

C2K = lambda c: 273.15 + c

ambiente1 = [
    5.0,  # L: comprimento do fio
    0.0,  # a: posição do início do fio
    2.0,  # tf: duração temporal simulada
    
    'C2K(20.0) if 2 < x < 4 else C2K(10.0)', # f(x)
    
    'C2K(0.0)', # M(t): temperatura do meio ambiente
    
    '-C2K(10.0)', # h(t): func. de contorno esquerda
    1.0, # alpha: coef. de T no lado esquerdo
    -1.0, # rho:   coef. de dT no lado esquerdo
    
    'C2K(10.0)', # g(t): func. de contorno direita
    1.0, # beta:  coef. de T no lado direito
    1.0, # sigma: coef. de dt no lado direito
    
    1.0,  # p: difusividade térmica do fio
    -0.0, # q: coef. de absorção da temp. do fio pelo meio ambiente
    -0.0, # r: coef. de geração de temp.
    
    25, # Nx: número de discretizações espaciais
    31, # Nt: número de discretizações temporais
    ]

custom = [
    5.0,  # comprimento do fio
    0.0,  # posição do início do fio
    2.0, # duração temporal simulada
    
    '20.0 if 2 < x < 4 else 10.0', # f(x)
    
    '5.0', # M(t) - temperatura do meio ambiente
    
    '0.0', # h(t)
    1.0, # coef. de T no lado esquerdo
    0.1,   # coef. de dT no lado esquerdo
    
    '0.0', # g(t)
    1.0,  # coef. de T no lado direito
    0.1, # coef. de dt no lado direito
    
    1.0,  # difusividade térmica
    -1.0, # coef. de absorção da temp. do fio pelo meio ambiente
    -1.0, # coef. de geração de temp.
    
    25, # número de discretizações espaciais
    30, # número de discretizações temporais
    ]


L,a,tf,f,M,h,alpha,rho,g,beta,sigma,p,q,r,Nx,Nt = ambiente1
f = eval('lambda x:'+f)
h = eval('lambda t:'+h)
g = eval('lambda t:'+g)
M = eval('lambda t:'+M)


# criando algumas constantes que serão necessárias
dx = L/Nx
dt = tf/Nt
fn = lambda n: f(a + n*dx)
Nx1 = Nx + 1

if abs(alpha*dx - rho) < 1e-7:
    sys.exit('alpha*dx - rho == 0: {0}*{1} - {2}'.format(alpha, dx, rho))


paleta = 'afmhot'

T = []
T0 = []
T.append(T0)
T0.append((dx*h(0) - rho*fn(1))/(alpha*dx - rho))
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

print '--'*32
print 'Função f(x) que define as temperaturas iniciais no fio'
draw(np.matrix([T0]), paleta)

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
            val = 2*dx*dx + 2*p*dt - q*dx*dt
        elif (col == lin + 1):
            val = -p*dt
        
        if (lin == 1 and col == 1):
            val = val + (rho*p*dt)/(alpha*dx - rho)
        elif (lin == Nx and col == Nx):
            val = val - (sigma*p*dt)/(beta*dx + sigma)
        
        cols.append(val)
    lines.append(cols)
matA = np.matrix(lines)

print '--'*32
print 'Matriz A do sistema Ax=b, que será aplicada sucessivamente'
draw(matA, cm.coolwarm)

# montando o vetor resultado do sistema
# esse vetor varia com o tempo, e deve ser remontado a cada iteração de tempo
def calcNextTimeResultVec(i):
    result = []
    for k in xrange(1, Nx1):
        res = p*dt*(TT(k - 1, i) + TT(k + 1, i))
        res += TT(k, i)*(2*dx*dx - 2*p*dt + q*dx*dt)
        res -= dx*dt*(M(i*dt) + M((i+1)*dt))*(q - r*dx)
        if (k == 1):
            res = res + (dx*h((i + 1)*dt)*p*dt)/(alpha*dx - rho)
        elif (k == Nx):
            res = res + (dx*g((i + 1)*dt)*p*dt)/(beta*dx + sigma)
        result.append([res])
    return np.matrix(result)

def drawVec(data, cmap):
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    ax1.imshow(data, interpolation="nearest", cmap=cmap, vmin=vmin, vmax=vmax)
    plt.show()

#drawVec(np.matrix([T0]), paleta)
# resolvendo iterativamente cada passo no tempo
for t in xrange(1, Nt):
    vecB = calcNextTimeResultVec(t - 1)
    Ti = la.solve(matA, vecB)
    Ti = list(Ti.flat)
    #print Ti
    Ti[:0] = [(dx*h(t*dt) - rho*Ti[0])/(alpha*dx - rho)]
    Ti.append((dx*g(t*dt) + sigma*Ti[-1])/(beta*dx + sigma))
    #drawVec(np.matrix([Ti]), paleta)
    T.append(Ti)

print '--'*32
print 'Evolução temporal de temperaturas no fio'
matT = np.matrix(T)
draw(matT, paleta)
