# -*- coding: utf-8 -*-
"""
Aluno: Miguel Angelo Santos Bicudo
DRE: 116033119
Curso: BCC
Data: 04/10/2016

Resolvendo equação do 2o grau e analisando condicionamento
"""

import numpy as np
import math as math

di = np.finfo(np.dtype('Float64'))
print di

print '\nEntre com os coeficientes da equação do segundo grau: ax^2 + bx + c = 0'
a, b, c = np.float64(input(""))
print '\nOs valores entrados foram:', '\n a = ', a, '\n b = ', b, '\n c = ', c

# Verificando condicionamento da composição que será usada para resolver o problema
if (b > 10000000):
    print 'Valor de b é muito grande'
elif (abs(a) < 0.0001):
    print 'Valor de a é muito próximo de 0'
elif (b*b < 4*a*c):
    print 'Valor de b*b deve ser maior que 4*a*c'
else:
    if (b*b > 4*a*c*0.9999 and b*b < 4*a*c*1.0001):
        print 'Valor de b*b muito próximo de 4*a*c'
        print 'Apesar do uso ser legítimo, pode acabar propagando um grande erro absoluto'
    d = math.sqrt(b*b - 4*a*c)
    p = (b + d)/a/2
    q = (b - d)/a/2
    print '\nAs raizes são: ', p, ' e ', q
