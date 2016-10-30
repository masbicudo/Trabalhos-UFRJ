# -*- coding: utf-8 -*-
"""
Created on Sat Oct 15 16:09:04 2016

@author: Miguel Angelo
"""

import numpy as np
import matplotlib.pyplot as plt



def cubic(a,b,c,d):
    return lambda x: a*x*x*x + b*x*x + c*x + d

def fn_sum(f1,f2):
    return lambda x: f1(x) + f2(x)

def slope_fwd(f,h):
    return lambda x: (f(x+h)-f(x))/h

def slope_bwd(f,h):
    return lambda x: (f(x)-f(x-h))/h

def slope_avg(f,h):
    return lambda x: (f(x+h)-f(x-h))/2/h

def tan_line(f,sf):
    return lambda xf: (lambda x: sf(xf)*(x-xf) + f(xf))

f = cubic(+1.0, -2.0, -4.0, +10.0)
vf = np.vectorize(f)

saf1 = slope_avg(f,0.5)
saf2 = slope_avg(f,0.2)
saf3 = slope_avg(f,0.05)
vsaf1 = np.vectorize(saf1)
vsaf2 = np.vectorize(saf2)
vsaf3 = np.vectorize(saf3)

tsaf1 = tan_line(f,saf1)
tsaf2 = tan_line(f,saf2)
tsaf3 = tan_line(f,saf3)
k = -0.0
vtsaf1 = np.vectorize(tsaf1(k))
vtsaf2 = np.vectorize(tsaf2(k))
vtsaf3 = np.vectorize(tsaf3(k))



X = np.linspace(-3.0, +3.0, 256, endpoint=True)

ax = plt.gca()  # gca stands for 'get current axis'
ax.spines['right'].set_color('none')
ax.spines['top'].set_color('none')
ax.xaxis.set_ticks_position('bottom')
ax.spines['bottom'].set_position(('data',0))
ax.yaxis.set_ticks_position('left')
ax.spines['left'].set_position(('data',0))

plt.plot(X, vf(X))

#plt.plot(X, vsaf1(X))

plt.plot(X, vtsaf1(X))
