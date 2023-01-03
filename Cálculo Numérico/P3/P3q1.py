# -*- coding: utf-8 -*-
"""
Created on Sat Dec 17 13:24:11 2016
"""

import sys as sys
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import cm as cm
import scipy.linalg as la
import matplotlib.animation as animation
import scipy.ndimage as im
from pylab import *
from mpl_toolkits.mplot3d import Axes3D

variaveis = [
    (5.0, 5.0),  # dimensoes do dominio D: (b, d)
    (0.0, 0.0),  # posicao da origem do dominio D: (a, c)
    'y+sin(t)', # f(t,y)
    40, # Nt
    40, # Ny
    ]

# setup inicial
Dl,Do,f,Nt,Ny = variaveis
f = eval('lambda t, y:'+f)
a, c = np.array(Do)
b, d = np.array(Do)+np.array(Dl)
dt = (b-a)/Nt
dy = (d-c)/Ny

paleta = 'afmhot'

## questao 1-a
## ===========
T = np.arange(a, b, dt)
Y = np.arange(c, d, dy)
T, Y = np.meshgrid(T, Y)
Z = f(T, Y)

fig = plt.figure()
ax = Axes3D(fig)
ax.plot_surface(T, Y, Z, rstride=1, cstride=1, cmap=paleta)
plt.show()

# questao 1-b
# ===========
Nniv = 10

szmul = 1
ti = a; tf = b; numt = Nt*szmul
yi = c; yf = d; numy = Ny*szmul

t = np.arange(ti, tf+1, (tf+1-ti)/(numt))
y = np.arange(yi, yf+1, (yf+1-yi)/(numy))
T,Y = np.meshgrid(t, y)
Z = f(T, Y)
zflat = Z.flatten()

vmin = min(zflat)
vmax = max(zflat)
extent = (a, b, c, d)
fig,ax = plt.subplots()
cax = ax.imshow(Z, extent=extent, cmap=paleta, interpolation="nearest", vmin=vmin, vmax=vmax)
c1 = ax.contour(T, Y, Z, levels=np.arange(vmin, vmax, (vmax-vmin)/Nniv), colors='k', vmin=vmin, vmax=vmax)
c2 = ax.contourf(T, Y, Z, levels=np.arange(vmin, vmax, (vmax-vmin)/Nniv), cmap=paleta, vmin=vmin, vmax=vmax)
fig.colorbar(cax)
plt.show()
