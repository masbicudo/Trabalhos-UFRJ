# -*- coding: utf-8 -*-
"""
Created on Sun Oct 30 17:45:43 2016

@author: Miguel Angelo
"""

import sys as sys
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import cm as cm
import scipy.linalg as la
import matplotlib.animation as animation
import scipy.ndimage as im
from pylab import *

print [1] + [2]

# Symmetric positive definite banded matrix `a`
a = array([[4.0, 1.0, 0.0, 0.0],
            [1.0, 4.0, 0.5, 0.0],
            [0.0, 0.5, 4.0, 0.2],
            [0.0, 0.0, 0.2, 4.0]])

# Banded storage form of `a`.
ab = array([[-1.0, 1.0, 0.5, 0.2],
             [4.0, 4.0, 4.0, 4.0]])
c = cholesky_banded(ab, lower=False, check_finite=False)
ufac = zeros_like(a)
ufac[list(range(4)),list(range(4))] = c[-1]
ufac[(0,1,2),(1,2,3)] = c[0,1:]
assert_array_almost_equal(a, dot(ufac.T, ufac))

b = array([0.0, 0.5, 4.2, 4.2])
x = cho_solve_banded((c, False), b, check_finite=False)
assert_array_almost_equal(x, [0.0, 0.0, 1.0, 1.0])
