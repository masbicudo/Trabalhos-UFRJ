# encoding=utf8  
"""
Created on Sat Oct 15 16:09:04 2016
"""

import sys as sys
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import cm as cm
import scipy.linalg as la
import matplotlib.animation as animation
import scipy.ndimage as im
from pylab import *

C2K = lambda c: 273.15 + c

ambiente1 = [
    (5.0, 5.0),  # L: dimensões da chapa
    (0.0, 0.0),  # a: posição da origem da chapa
    20.0,  # tf: duração temporal simulada
    
    '0.0 if t > 0.0 else C2K(200.0) if 2 < x < 4 and 2 < y < 4 else C2K(0.0)', # f(x,y,t)
    
    C2K(5.0), # M: temperatura do meio ambiente
    
    C2K(115.0), # Te: 
    C2K(5.0), # Td: 
    C2K(115.0), # Ts: 
    C2K(5.0), # Ti: 
    
    1.0,  # p: difusividade térmica da chapa
    -0.0, # q: coef. de absorção da temp. da chapa pelo meio ambiente
    -0.0, # r: coef. de geração de temp.
    
    6, # Nx: número de discretizações espaciais em x
    6, # Ny: número de discretizações espaciais em y
    200, # Nt: número de discretizações temporais
    ]

paleta = 'afmhot'

L,a,tf,f,M,Te,Td,Ts,Ti,p,q,r,Nx,Ny,Nt = ambiente1
Lx,Ly=L
x0,y0=a
f = eval('lambda x, y, t:'+f)


# criando algumas constantes que serão necessárias
dx = Lx/Nx
dy = Ly/Ny
dt = tf/Nt
fn = lambda j, k, i: f(x0 + j*dx, y0 + k*dy, i*dt)


T = [[]]
for k in xrange(1, Ny+1):
    for j in xrange(1, Nx+1):
        T[0].append(fn(j,k,0))

vmin = min(T[0])
vmax = max(T[0])

def draw(data, cmap):
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    cax = ax1.imshow(data, interpolation="nearest", cmap=cmap)
    fig.colorbar(cax, shrink=0.75, aspect=5)
    ax1.grid(True)
    plt.show()

def drawPlate(data, cmap):
    data = np.reshape(data, (Nx, Ny))
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    ax1.imshow(data, interpolation="nearest", cmap=cmap, vmin=0.0, vmax=vmax)
    plt.show()

print '--'*32
print 'Função f(x) que define as temperaturas iniciais na chapa'
drawPlate(T[0], paleta)

def getIndex(j,k):
    return j-1 + (k-1)*Nx

def TT(j,k):
    #print (j,k)
    if j==0 and k==0: return 0.0
    if j==0 and k==Ny+1: return 0.0
    if j==Nx+1 and k==0: return 0.0
    if j==Nx+1 and k==Ny+1: return 0.0
    if j==0: return Te
    if j==Nx+1: return Td
    if k==0: return Ts
    if k==Ny+1: return Ti
    return T[0][getIndex(j,k)]

# montando a matriz que representa o sistema a ser resolvido
# essa matriz não varia com o tempo
N = Nx*Ny
bandsDic = {}
def set_banded(lin, col, val):
    key = abs(lin-col)
    if (abs(val) <= 1e-7):
        if (key in bandsDic):
            bandsDic[key][max((lin,col))] = 0.0
    else:
        if (not key in bandsDic):
            bandsDic[key] = [-1.0]*N
        bandsDic[key][max((lin,col))] = val

for linY in xrange(1, Ny+1):
    for linX in xrange(1, Nx+1):
        for colY in xrange(1, Ny+1):
            for colX in xrange(1, Nx+1):
                val = 0.0
                
                if ((colX, colY) == (linX - 1, linY)):
                    val = -p*dt*dy*dy
                elif ((colX, colY) == (linX + 1, linY)):
                    val = -p*dt*dy*dy
                elif ((colX, colY) == (linX, linY - 1)):
                    val = -p*dt*dx*dx
                elif ((colX, colY) == (linX, linY + 1)):
                    val = -p*dt*dx*dx
                elif ((colX, colY) == (linX, linY)):
                    val = 4*dx*dx*dy*dy + 2*p*dt*(dx*dx + dy*dy) - q*dt*dx*dy*(dx + dy)
                
                set_banded(getIndex(linX, linY), getIndex(colX, colY), val)


bandKeys = range(0, max(bandsDic.keys())+1)
bandKeys.sort()
bandKeys.reverse()
bands = []
for key in bandKeys:
    if (key in bandsDic):
        bands.append(bandsDic[key])
    else:
        bands.append([-1.0]*key + [0.0]*(N-key))
bands = np.matrix(bands)
#print 'bands:', bands

choA = la.cholesky_banded(bands, lower=False)


lines = []
for linY in xrange(1, Ny+1):
    for linX in xrange(1, Nx+1):
        cols = []
        for colY in xrange(1, Ny+1):
            for colX in xrange(1, Nx+1):
                val = 0.0
                
                if ((colX, colY) == (linX - 1, linY)):
                    val = -p*dt*dy*dy
                elif ((colX, colY) == (linX + 1, linY)):
                    val = -p*dt*dy*dy
                elif ((colX, colY) == (linX, linY - 1)):
                    val = -p*dt*dx*dx
                elif ((colX, colY) == (linX, linY + 1)):
                    val = -p*dt*dx*dx
                elif ((colX, colY) == (linX, linY)):
                    val = 4*dx*dx*dy*dy + 2*p*dt*(dx*dx + dy*dy) - q*dt*dx*dy*(dx + dy)
                                
                cols.append(val)
        lines.append(cols)
matA = np.matrix(lines)

print '--'*32
print 'Matriz A do sistema Ax=b, que será aplicada sucessivamente'
draw(matA, cm.coolwarm)


#print matA
#print np.shape(matA)
#print 'Is symmetric:', (matA.transpose() == matA).all()
#print 'Is positive definite:', np.all(np.linalg.eigvals(matA) > 0)
#print 'Eigenvalues:', np.linalg.eigvals(matA)


#choFacA = la.cho_factor(matA)

# montando o vetor resultado do sistema
# esse vetor varia com o tempo, e deve ser remontado a cada iteração de tempo
def calcNextTimeResultVec(i):
    result = []
    for k in xrange(1, Ny+1):
        for j in xrange(1, Nx+1):
            res = fn(j,k,i+1)
            #res = 0.0
            
            res += p*dt*dy*dy*(TT(j+1, k) + TT(j-1, k))
            res += p*dt*dx*dx*(TT(j, k+1) + TT(j, k-1))
            res += (4*dx*dx*dy*dy - 2*p*dt*(dx*dx + dy*dy) + q*dt*dx*dy*(dx + dy))*TT(j, k)
            #print ''
            res += 4*r*dt*dx*dx*dy*dy*M
            
            if (j == 1):
                res = res + Te*p*dt*dy*dy
            elif (j == Nx):
                res = res + Td*p*dt*dy*dy
            
            if (k == 1):
                res = res + Ts*p*dt*dy*dy
            elif (k == Ny):
                res = res + Ti*p*dt*dy*dy
            
            result.append([res])
    return np.matrix(result)


##############################################################################
# ANIMAÇÃO
##############################################################################
szmul = 1
xi = x0; xf = x0+Lx; numx = Nx*szmul
yi = y0; yf = y0+Ly; numy = Ny*szmul
xlabeltext = r'x'; ylabeltext = r'y'
titletext = r'Simulacao'

fig,ax = plt.subplots()

x = np.arange(xi, xf+1, (xf+1-xi)/(numx))
y = np.arange(yi, yf+1, (yf+1-yi)/(numy))
X,Y = np.meshgrid(x, y)

def drawFrame(i):
    #print ''
    print 'i:',i
    gridT = np.reshape(T[0], (Nx, Ny))
    #print 'gridT:', shape(gridT)
    #print gridT
    #print 'x:',shape(x)
    #print 'y:',shape(y)
    z = im.zoom(gridT, szmul, order=3)
    extent = (xi, xf, yf, yi)
    ax.imshow(z, extent=extent, cmap=paleta, interpolation="nearest", vmin=0.0, vmax=vmax)
    c1 = ax.contour(x,y,z, levels=np.arange(0.0, vmax, vmax/50.0), colors='k', vmin=0.0, vmax=vmax)
    c2 = ax.contourf(x,y,z, levels=np.arange(0.0, vmax, vmax/50.0), cmap=paleta, vmin=0.0, vmax=vmax)
    
    # calculando os valores para o próximo frame da animação
    vecB = calcNextTimeResultVec(i)
    #print vecB
    #vecR = la.solve(matA, vecB)
    #print 'choFacA:', choFacA
    vecR = la.cho_solve_banded((choA, False), vecB)
    #vecR = la.cho_solve(choFacA, vecB)
    vecR = list(vecR.flat)
    #print 'vecR:', vecR
    T[0] = vecR

# função de animação de cada frame
cntFrame = [-1]
def animate(i):
    if (i > cntFrame[0]):
        ax.clear()
        drawFrame(i)
        cntFrame[0] = i
    return ax,

# gerando o primeiro frame
plt.grid(True)
plt.xlabel(xlabeltext); plt.ylabel(ylabeltext); plt.title(titletext)
drawFrame(0)

# animando do segundo frame em diante
ani = animation.FuncAnimation(fig, animate, np.arange(1, Nt), interval=1000*dt, blit=True)

plt.show()
