# -*- coding: utf-8 -*-
"""
Created on Wed Dec 21 13:36:11 2016
"""

import numpy as np
import scipy.sparse.linalg as la
import numpy.linalg as li

A0 = np.array([
    [1.0, 0.0, 1.0,0],
    [0.0, 1.0, 1.0,0],
    [3.0, 2.0, 0.0,0]])*2

# A funcao svds nao consegue retornar todos os valores singulares
# por isso, adiciono uma coluna e uma linha zeradas, que vai
# causar a adicao de zeros nas matrizes unitarias direita e esquerda,
# alem de adicionar um zero aos valores singulares.
# Como svds retorna os maiores valores singulares, entao o zero
# adicionado sera descartado pois sera o menor com certeza.
# Nenhum valor singular eh menor que zero, pois os valores singulares
# representam um esticamento. Esticamentos negativos sao apenas inversoes
# e portanto acabam fazendo parte das matrizes esquerda e/ou direita,
# o que tornaria o valor singular positivo.
A1 = np.c_[A0, np.zeros((np.size(A0, 0), 1))]
A1 = np.r_[A1, np.zeros((1, np.size(A1, 1)))]

vSE, sigma, vSD = la.svds(A1, min(A1.shape)-1)
# obter A de volta: np.dot(vSE, np.dot(np.diag(sigma), vSD))

# A inversa eh necessaria para calcular as normas 1 e infinita.
# Vou usar a SVD para calcular a pseudo-inversa que eh igual a inversa
# para matrizes quadradas.
pinv = np.dot(np.transpose(vSD), np.dot(np.diag(1./sigma), np.transpose(vSE)))
pinv = pinv[0:-1,0:-1]

# Referencias
# - sobre normas: http://faculty.nps.edu/rgera/MA3042/2009/ch7.4.pdf

# A norma 1 eh o maximo entre as somas de valores absolutos de cada coluna.
n1A = max(np.sum(np.abs(A0), axis=0))
n1iA = max(np.sum(np.abs(pinv), axis=0))
print("Numero de Condicao 1")
print n1A*n1iA#, '; li.cond(A0, 1) =', li.cond(A0, 1)
print("Norma 1")
print n1A#, '; li.norm(A0, 1) =', li.norm(A0, 1)

# A norma 1 eh o maximo entre as somas de valores absolutos de cada linha.
nInfA = max(np.sum(np.abs(A0), axis=1))
nInfInvA = max(np.sum(np.abs(pinv), axis=1))
print("Numero de Condicao Infinito")
print nInfA*nInfInvA#, '; li.cond(A0, np.inf) =', li.cond(A0, np.inf)
print("Norma Infinita")
print nInfA#, '; li.cond(A0, np.inf) =', li.norm(A0, np.inf)

# A norma 2 pode ser obtida de varias maneiras
# uma delas eh usando a norma 2 das matrizes
# A e inversa de A. Acontece que a norma 2 de
# A eh o maior valor singular, enquanto que a
# norma 2 da inversa de A eh o inverso do 
# menor valor singular de A.
n2A = max(sigma)
n2iA = 1/min(sigma)
print("Numero de Condicao 2")
print n2A*n2iA#, '; li.cond(A0, 2) =', li.cond(A0, 2)
print("Norma 2")
print n2A#, '; li.norm(A0, 2) =', li.norm(A0, 2)
