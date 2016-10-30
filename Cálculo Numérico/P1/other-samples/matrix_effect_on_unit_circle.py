# -*- coding: utf-8 -*-
"""
Created on Sat Oct 15 13:55:23 2016

@author: Miguel Angelo
"""

import numpy as np
import matplotlib.pyplot as plt

tr = 45*'-'
print 'Entre com o intervalo [a, b] do parâmetro t:'
a, b = np.float32(input('a, b = '))
print tr
print 'Agora entre com o número N de vetores:'
N = np.int32(input('N = '))
print tr
T = np.linspace(a,b,N)
def x(t):
    return np.cos(np.pi*t)
def y(t):
    return np.sin(np.pi*t)
X = x(T)
Y = y (T)
XY = np.mat([X,Y])
print 'Entre com a matriz M (sintaxe Matlab):'
M = np.mat(raw_input('M = '))
print tr, '\nM =\n', M, '\n', tr
M_XY = M*XY
M_X = np.ravel(M_XY[0])
M_Y = np.ravel(M_XY[1])
x_m = min(np.amin([X, M_X]), 0.0)
x_M = max(np.amax([X, M_X]), 0.0)
y_m = min(np.amin([Y, M_Y]), 0.0)
y_M = max(np.amax([Y, M_Y]), 0.0)
f_x = (x_M - x_m)/10.0
f_y = (y_M - y_m)/10.0
print 'Veja o gráfico da ação de M'

for r,s in zip(X,Y):
    plt.arrow(0.0, 0.0, r, s, width=0.003, head_width=0.03, color='r')
for u,v in zip(M_X, M_Y):
    plt.arrow(0.0, 0.0, u, v, width=0.003, head_width=0.03, color='r', ls='dotted')

plt.axhline(y=0.0, color='k')
plt.axvline(x=0.0, color='k')
plt.grid(True)
plt.axis([x_m - f_x, x_M + f_x, y_m - f_y, y_M + f_y])
plt.title('Ação da matriz M num feixe de vetores')
plt.show()
