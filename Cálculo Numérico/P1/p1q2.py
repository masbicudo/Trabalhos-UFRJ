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
f = eval("lambda x:"+input("fn(x): "), {"sen": math.sin, "cos": math.cos})
df = eval("lambda x:"+input("Df(x): "), {"sen": math.sin, "cos": math.cos})
a = eval(input('Insira o ponto inicial x0:\na = '))
N = eval(input('Insira o número de pontos\n:N = '))
ev = eval(input('Insira a escala vertical:\nE = '))
k = np.arange(0, N+1, 1)




dfa = df(a)

vff = []
vaf = []
vbf = []
eff = []
eaf = []
ebf = []
for n in k:
    h = pow(2.0, -n)
    vff.append(slope_fwd(f, h)(a))
    eff.append(abs(vff[-1]-dfa))
    vaf.append(slope_avg(f, h)(a))
    eaf.append(abs(vaf[-1]-dfa))
    vbf.append(slope_bwd(f, h)(a))
    ebf.append(abs(vbf[-1]-dfa))

def grafico(e):
    plt.plot(k, np.repeat(dfa, N+1), 'k-')
    plt.plot(k, vff, 'go')
    plt.plot(k, vaf, 'ro')
    plt.plot(k, vbf, 'bo')
    
    plt.axis([0, N, dfa - e, dfa + e])
    
    plt.grid(True)
    plt.show()

def graficolog():
    ax = plt.gca()
    ax.set_yscale('log')
    plt.plot(k, eff, 'go')
    plt.plot(k, eaf, 'ro')
    plt.plot(k, ebf, 'bo')
    
    plt.grid(True)
    plt.show()

grafico(5e-3)
grafico(5e-5)
grafico(5e-7)
graficolog()
