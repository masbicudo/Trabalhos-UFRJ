# encoding=utf8  
"""
Created on Sat Oct 15 16:09:04 2016
"""

import matplotlib.cm as cm

C2K = lambda c: 273.15 + c

# Configuracoes de usuario
# ========================

class ambiente1_modelo1:
    t = 1.5
    l = 1.
    r = 4.2
    b = 5.9
    r2 = 2.7
    t2 = 4.4
    
    Tt = 300
    Tl = 340
    Tr = 390
    Tb = 450
    Tr2 = 520
    Tt2 = 600

ambiente1 = [
    ambiente1_modelo1(), # m: modelo para gerar a figura
    
    (5.0, 7.0),  # L: dimensões do domínio
    (0.0, 0.0),  # a: posição da origem do domínio
    20.0,  # tf: duração temporal simulada
    
    # sh(x,y): funcao que indica a forma da figura
    #   m: é o modelo utilizado para gerar a figura
    'not (x < m.l or y < m.t or x > m.r or y > m.b or (x > m.r2 and y < m.t2))',

    # f(x,y,t): temperaturas geradas no interior da figura no tempo t
    #   m: é o modelo utilizado para gerar a figura
    'C2K(0.0) if t == 0 else 0.0',

    # M(x,y,t): temperatura de contorno ou do meio ambiente
    #   m: é o modelo utilizado para gerar a figura
    #   sh: funcao que determina a forma da figura
    # Notas adicionais:
    #   tudo que estiver fora da figura não influencia na mesma,
    #   se estiver na borda da figura, ai será considerada como contorno,
    #   se estiver dentro da figura, considera-se como ambiente
    'C2K(5.0) if sh(x,y) else m.Tl if x < m.l else m.Tt if y < m.t else m.Tr if x > m.r else m.Tb if y > m.b else m.Tr2 if x-m.r2 < -(y-m.t2) else m.Tt2',

    1.0,  # p: difusividade térmica da chapa
    -0.0, # q: coef. de absorção da temp. da chapa a partir do meio ambiente
    -0.0, # r: coef. de geração de temp.
    
    20, # Nx: número de discretizações espaciais em x
    20, # Ny: número de discretizações espaciais em y
    200, # Nt: número de discretizações temporais (usado para calcular dt)
    ]

paleta = cm.afmhot

# Setup do programa
# =================

import sys as sys
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import cm as cm
import scipy.linalg as la
import matplotlib.animation as animation
import scipy.ndimage as im
from pylab import *
from scipy.sparse import spdiags

#constantes
fundo = -1.0
borda = +1.0
forma = +2.0

# decodificando variaveis do usuario
m,L,a,tf,sh,f,M,p,q,r,Nx,Ny,Nt = ambiente1
Lx,Ly=L
x0,y0=a
sh = eval('lambda x, y:'+sh)
f = eval('lambda x, y, t:'+f)
M = eval('lambda x, y, t:'+M)
N = Nx*Ny

# criando algumas constantes que serão necessárias
dx = Lx/Nx
dy = Ly/Ny
dt = tf/Nt
shn = lambda j, k: sh(x0 + j*dx, y0 + k*dy)
fn = lambda j, k, i: f(x0 + j*dx, y0 + k*dy, i*dt) if shn(j, k) else np.nan
Mn = lambda j, k, i: M(x0 + j*dx, y0 + k*dy, i*dt)

vfMn = np.vectorize(Mn)
vffn = np.vectorize(fn)
X, Y = np.meshgrid(range(0, Nx), range(0, Ny))
xs = X.flatten()
ys = Y.flatten()

# funcoes ajudantes
import copy

def drawPlate(data, cmap='afmhot', title='', transp=True, vmax=False):
    data = np.reshape(data, (Ny, Nx))
    fig = plt.figure()
    ax1 = fig.add_subplot(1,1,1)
    cmap = copy.copy(plt.cm.get_cmap(cmap) if type(cmap) is str else cmap) # copiando o cmap
    cmap.set_bad(alpha=0.) # definindo o que acontece com valores NaN
    if (transp):
        cmap.set_under('k', alpha=0.)
    ax1.patch.set_facecolor('red') # definindo fundo do plot preto
    ax1.patch.set_hatch('\\\\\\\\////')
    #ax1.patch.set_fill(None)
    ax1.patch.set_alpha(0.5) # definindo fundo do plot com alpha=1 (opaco)
    if (not type(vmax) is bool):
        ax1.imshow(data, interpolation="nearest", cmap=cmap, vmin=0.0, vmax=vmax)
    else:
        ax1.imshow(data, interpolation="nearest", cmap=cmap)
    ax1.set_title(title)
    # ajustando aspecto da imagem de acordo com L do domínio
    aspect = Lx/Ly
    img = ax1.get_images()
    extent =  img[0].get_extent()
    ax1.set_aspect(abs((extent[1]-extent[0])/(extent[3]-extent[2]))/aspect)
    # mostrando o plot
    plt.show()

# programa
mapa = []
for k in xrange(0, Ny):
    for j in xrange(0, Nx):
        mapa.append(2.0 if shn(j,k) else 1.0 if shn(j+1,k) or shn(j-1,k) or shn(j,k+1) or shn(j,k-1) else -1.0)
mapa = np.array(mapa)

# grafico 1 - formato da figura
bmp_shape = np.reshape(mapa, (Ny, Nx))
drawPlate(bmp_shape, paleta, u'Função sh(x,y) da forma da figura')

# contando quantos pontos sao internos ou de contorno
n = np.sum(bmp_shape > 0.0)
print 'Quantidade de pontos de contorno ou dentro: ', n, ' de ', np.size(mapa)

# Montando a matriz esparsa A
# ===========================


# a diagonal principal eh a relacao do ponto com ele mesmo
diag0 = np.ones(size(mapa))
diag0[mapa == fundo] = 1.0
diag0[mapa == borda] = 1.0
diag0[mapa == forma] = 4*dx*dx*dy*dy + 2*p*dt*(dx*dx + dy*dy) - q*dt*dx*dy*(dx + dy)
drawPlate(diag0, paleta, u'Diagonal principal', False)

# a diagonal +1 eh a relacao do ponto com o da direita
diag_u1 = np.ones(size(mapa) - 1)
diag_u1[mapa[0:N-1] == fundo] = 0.0
diag_u1[mapa[0:N-1] == borda] = 0.0
diag_u1[mapa[0:N-1] == forma] = -p*dt*dy*dy
drawPlate(np.append([np.nan], diag_u1), paleta, u'Diagonal +1: ponto a direita', False)

# a diagonal -1 eh a relacao do ponto com o da esquerda
diag_l1 = np.ones(size(mapa) - 1)
diag_l1[mapa[1:N] == fundo] = 0.0
diag_l1[mapa[1:N] == borda] = 0.0
diag_l1[mapa[1:N] == forma] = -p*dt*dy*dy
drawPlate(np.append(diag_l1, [np.nan]), paleta, u'Diagonal -1: ponto a esquerda', False)

# a diagonal +Nx eh a relacao do ponto com o de baixo
diag_uNx = np.ones(size(mapa) - Nx)
diag_uNx[mapa[0:(N-Nx)] == fundo] = 0.0
diag_uNx[mapa[0:(N-Nx)] == borda] = 0.0
diag_uNx[mapa[0:(N-Nx)] == forma] = -p*dt*dx*dx
drawPlate(np.append(np.ones(Nx)*np.nan, diag_uNx), paleta, u'Diagonal +Nx: ponto abaixo', False)

# a diagonal -Nx eh a relacao do ponto com o de cima
diag_lNx = np.ones(size(mapa) - Nx)
diag_lNx[mapa[Nx:N] == fundo] = 0.0
diag_lNx[mapa[Nx:N] == borda] = 0.0
diag_lNx[mapa[Nx:N] == forma] = -p*dt*dx*dx
drawPlate(np.append(diag_lNx, np.ones(Nx)*np.nan), paleta, u'Diagonal -Nx: ponto acima', False)

# criando a matriz esparsa com as diagonais
from scipy.sparse import diags
diagonals = [diag0, diag_u1, diag_l1, diag_uNx, diag_lNx]
matA = diags(diagonals, [0, +1, -1, +Nx, -Nx])

# Mostrando normas e numeros de condicionamento
# =============================================

import scipy.sparse.linalg as la
A0 = matA

print 'Questao 3: numeros de condicionamento e normas'

# A funcao svds nao consegue retornar todos os valores singulares,
# mais especificamente o ultimo e menor valor singular,
# por isso, adiciono uma coluna e uma linha zeradas, que vai
# causar a adicao de zeros nas matrizes unitarias direita e esquerda,
# alem de adicionar um zero aos valores singulares.
# Como svds retorna os maiores valores singulares, entao o zero
# adicionado sera descartado pois sera o menor com certeza.
# Nenhum valor singular eh menor que zero, pois os valores singulares
# representam um esticamento. Esticamentos negativos sao apenas inversoes
# e portanto acabam fazendo parte das matrizes esquerda e/ou direita,
# o que tornaria o valor singular positivo.
diagonals1 = [
    np.append(diag0, [0]),
    np.append(diag_u1, [0]),
    np.append(diag_l1, [0]),
    np.append(diag_uNx, [0]),
    np.append(diag_lNx, [0])]

matA1 = diags(diagonals1, [0, +1, -1, +Nx, -Nx])
A1 = matA1

if N < 500:
    vSE, sigma, vSD = la.svds(A1, min(A1.shape)-1)
    # obter A de volta: np.dot(vSE, np.dot(np.diag(sigma), vSD))
    # IMPORTANTE: note que as matrizes retornadas nao sao esparsas,
    # o que pode acarretar em alto uso de memoria do computador.
    # A unica norma capaz de evitar o uso da inversa eh a norma 2,
    # a qual pode ser calculada usando apenas os valores singulares,
    # sendo possivel chamar:
    # sigma = la.svds(A1, min(A1.shape)-1, return_singular_vectors = False)
    
    # A inversa eh necessaria para calcular as normas 1 e infinita.
    # Vou usar a SVD para calcular a pseudo-inversa que eh igual a inversa
    # para matrizes quadradas.
    pinv = np.dot(np.transpose(vSD), np.dot(np.diag(1./sigma), np.transpose(vSE)))
    pinv = pinv[0:-1,0:-1]
    
    # Referencias
    # - sobre normas: http://faculty.nps.edu/rgera/MA3042/2009/ch7.4.pdf
    
    # A norma 1 eh o maximo entre as somas de valores absolutos de cada coluna.
    n1A = np.abs(A0).sum(axis=0).max()
    n1iA = np.abs(pinv).sum(axis=0).max()
    print("Numero de Condicao 1")
    print n1A*n1iA#, '; li.cond(A0, 1) =', li.cond(A0, 1)
    print("Norma 1")
    print n1A#, '; li.norm(A0, 1) =', li.norm(A0, 1)
    
    # A norma 1 eh o maximo entre as somas de valores absolutos de cada linha.
    nInfA = np.abs(A0).sum(axis=1).max()
    nInfInvA = np.abs(pinv).sum(axis=1).max()
    print("Numero de Condicao Infinito")
    print nInfA*nInfInvA#, '; li.cond(A0, np.inf) =', li.cond(A0, np.inf)
    print("Norma Infinita")
    print nInfA#, '; li.cond(A0, np.inf) =', li.norm(A0, np.inf)

else:
    sigma = la.svds(A1, min(A1.shape)-1, return_singular_vectors = False)

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


# Usando metodo iterativo para resolver sistema
# =============================================

# matrizes deslocadas da figura
def shiftRight(a, v=0):
    r = np.array(a[0:N-1])
    r[xs[0:N-1] == Nx-1] = v
    return np.append([v], r)

def shiftLeft(a, v=0):
    r = np.array(a[1:N])
    r[xs[1:N] == 0] = v
    return np.append(r, [v])

def shiftDown(a, v=0):
    r = np.array(a[0:N-Nx])
    return np.append(np.repeat(v, Nx), r)

def shiftUp(a, v=0):
    r = np.array(a[Nx:N])
    return np.append(r, np.repeat(v, Nx))


vecM = vfMn(xs, ys, 0)
vmin = min(vecM)
vmax = max(vecM)
drawPlate(vecM, paleta, u'M: temp. ambiente em t=0', False, vmax)

T = vffn(xs, ys, 0)
T[mapa == fundo] = 0
T[mapa == borda] = vecM[mapa == borda]

fatores = [
    p*dt*dy*dy,
    p*dt*dx*dx,
    4*dx*dx*dy*dy - 2*p*dt*(dx*dx + dy*dy) + q*dt*dx*dy*(dx + dy),
    4*r*dt*dx*dx*dy*dy,# falta multiplicar M que depende de (x,y)
    ]

temps = [
    shiftLeft(T) + shiftRight(T),
    shiftUp(T) + shiftDown(T),
    T,
    vecM,
    ]

vecB = sum((fatores[k]*temps[k]) for k in (0,1,2,3))
vecB[mapa == fundo] = fundo
vecB[mapa == borda] = vecM[mapa == borda] #vfMn(xs[mapa == borda], ys[mapa == borda], 0)
drawPlate(vecB, paleta, u'b', False, vmax)

drawPlate(T, paleta, u'T: temp. em t=0', False, vmax)

from scipy.sparse.linalg import bicgstab
T, _ = bicgstab(matA, vecB)
drawPlate(T, paleta, u'T: temp. em t=' + str(1) + 'dt')

for it in xrange(1, 10):
    vecM = vfMn(xs, ys, it)
    T = T + vffn(xs, ys, it)
    T[mapa == fundo] = 0
    T[mapa == borda] = vecM[mapa == borda]
    temps = [shiftLeft(T) + shiftRight(T), shiftUp(T) + shiftDown(T), T, vecM]
    vecB = sum((fatores[k]*temps[k]) for k in (0,1,2,3))
    vecB[mapa == fundo] = fundo
    vecB[mapa == borda] = vecM[mapa == borda] #vfMn(xs[mapa == borda], ys[mapa == borda], 0)
    result, _ = bicgstab(matA, vecB)
    drawPlate(result, paleta, u'T: temp. em t=' + str(it+1) + 'dt')
