# based on sample by: Roberto Colistete Jr.
# extended with animation by: MASBicudo ()

import matplotlib.pyplot as plt
import numpy as np
import matplotlib.animation as animation
from pylab import *

##############################################################################
## Personalize your plot parameters here.
## x values from xi to xf with numx points being sampled :
xi = -2.0; xf = 2.0; numx = 150
## y values from yi to yf with numy points being sampled :
yi = -2.0; yf = 2.0; numy = 150
## z = f(x,y), just change the expression after 'return' :
def f(x,y,i):
     return (cos(pi*x*y)**2+i/20.0)*exp(-(x**2+y**2)/2)
## Option (True/False) to fill the gaps between contours :
contourfillflag = True
## Option (True/False) to only fill the gaps between contours, without drawing 
## the contours :
contourfillonlyflag = False
## Contour levels can be True (automatic), the number of leves or the list of 
## z levels :
# contourlevels = True
contourlevels = 20
# contourlevels = linspace(0.0,1.0,10)
# contourlevels = (0.0,0.2,0.4,0.6,0.8,1.0)
## Option (True/False) to show automatic contour labels  :
contourlabelsflag = False
## Option (True/False) to show a vertical color bar of the contour levels  :
contourlevelsbarflag = True
## Labels for x and y axis :
xlabeltext = r'x'; ylabeltext = r'y'
## Plot title, here including TeX expressions (inside '$') :
titletext = r'$f(x,y)=\,\cos(\pi\,x\,y)^2 e^{-\frac{x^2+y^2}{2}}$'
## Option (True/False) to show a grid of dashed lines :
gridflag = True
##############################################################################

fig,ax = plt.subplots()

x = np.arange(xi, xf, (xf-xi)/(numx-1))
y = np.arange(yi, yf, (yf-yi)/(numy-1))
X,Y = np.meshgrid(x, y)

def draw(i):
    z = f(X,Y,i)
    extent = (xi, xf, yi, yf)
    ax.imshow(z, extent=extent)
    if contourfillflag:
        if contourlevels == True:
            if not contourfillonlyflag:
                c1 = ax.contour(x,y,z,colors='k')
            c2 = ax.contourf(x,y,z)
        else:
            if not contourfillonlyflag:
                c1 = ax.contour(x,y,z,contourlevels,colors='k')
            c2 = ax.contourf(x,y,z,contourlevels)
        if contourlevelsbarflag and i == 0:
            cb2 = fig.colorbar(c2)
    else:
        if contourlevels == True:
            c1 = ax.contour(x,y,z)
        else:
            c1 = ax.contour(x,y,z,contourlevels)
        if contourlevelsbarflag and i == 0:
            fig.colorbar(c1)
    if contourlabelsflag and ((not contourfillflag) or (not contourfillonlyflag)) and i == 0:
        l1 = ax.clabel(c1)

plt.grid(gridflag)
plt.xlabel(xlabeltext); plt.ylabel(ylabeltext); plt.title(titletext)

def animate(i):
    ax.clear()
    draw(i)
    return ax,

draw(0)

ani = animation.FuncAnimation(fig, animate, np.arange(1, 200), interval=10, blit=True)

plt.show()
