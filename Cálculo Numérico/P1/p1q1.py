# -*- coding: utf-8 -*-
"""
Created on Sat Oct 15 16:09:04 2016
"""



import math
import numpy as np
import matplotlib as plt
from pylab import*


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



print ('Fórmula em função de x, é possível usar as funções sen e cos')
fntext = input("fn(x): ")
print (fntext)
f = eval("lambda x:"+fntext, {"sen": math.sin, "cos": math.cos})
inter = np.float32(eval(input('Insira o intervalo da Função: ')))
print(inter)

vf = np.vectorize(f)

saf1 = slope_fwd(f,0.5)
saf2 = slope_fwd(f,0.2)
saf3 = slope_fwd(f,0.05)
vsaf1 = np.vectorize(saf1)
vsaf2 = np.vectorize(saf2)
vsaf3 = np.vectorize(saf3)

tsaf1 = tan_line(f,saf1)
tsaf2 = tan_line(f,saf2)
tsaf3 = tan_line(f,saf3)
a = -0.0
vtsaf1 = np.vectorize(tsaf1(a))
vtsaf2 = np.vectorize(tsaf2(a))
vtsaf3 = np.vectorize(tsaf3(a))



X = np.linspace(inter[0], inter[1], 256, endpoint=True)

def grafico(h, vtsaf,cor):
    ax = plt.gca()  # gca stands for 'get current axis'
    ax.spines['right'].set_color('none')
    ax.spines['top'].set_color('none')
    ax.xaxis.set_ticks_position('bottom')
    ax.spines['bottom'].set_position(('data',0))
    ax.yaxis.set_ticks_position('left')
    ax.spines['left'].set_position(('data',0))

    plt.plot(X, vf(X), '-r', label = 'f(x)')
    plt.plot(X, vtsaf(X), cor, label = 'Derivada atrasada')
    plt.grid(True)
    plt.show()

grafico(0.5, vtsaf1, '-b')
grafico(0.2, vtsaf2, '-y')
grafico(0.05, vtsaf3, '-g')
