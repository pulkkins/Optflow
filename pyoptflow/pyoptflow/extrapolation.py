"""Methods for extrapolation of images by using a motion vector field."""

from numpy import arange, dstack, meshgrid, nan, reshape, size, zeros
from scipy.ndimage.interpolation import map_coordinates

def semilagrangian(I, V, t, n_steps, n_iter=3, inverse=True):
  """Apply semi-Lagrangian extrapolation to an image by using a motion field. 
  In this scheme, the extrapolation is done in a by integrating the motion 
  field. Differently to linear extrapolation, this allows modeling nonlinear 
  motion such as rotation. The pixels involving motion to/from outside the 
  image boundaries are set to NaN.
  
  Parameters
  ----------
  I : array-like
    The input image.
  V : array-like
    The motion field. If inverse=False, V is interpreted as backward motion 
    field, and a forward field otherwise. V must have the same shape as I.
  t : float
    Time step length for extrapolation (1.0 is the time difference between the 
    input images from which V has been computed).
  n_steps : int
    Number of intermediate time steps to use in the computation.
  n_iter : int
    Number of inner iterations in the semi-Lagrangian scheme.
  inverse : bool
    If True, the extrapolation trajectory is computed backward along the flow, 
    forward otherwise. Inverse extrapolation is the default, as it usually 
    gives better results.
  
  Returns
  -------
  out : ndarray
    The extrapolated image at time t.
  """
  if len(I.shape) != 2:
    raise ValueError("I must be a two-dimensional array")
  if I.shape != V.shape[0:2]:
    raise ValueError("I and V must have the same shape")
  
  coeff = 1.0 if inverse == False else -1.0
  
  delta_t = 1.0 * t / n_steps
  X,Y = meshgrid(arange(size(I, 1)), arange(size(I, 0)))
  XY  = dstack([X, Y])
  
  V_tot = zeros((V.shape[0], V.shape[1], 2))
  for i in range(n_steps):
    V_inc = zeros(V_tot.shape)
    for j in range(n_iter):
      XYW = XY + V_tot - V_inc / 2.0
      XYW = [XYW[:, :, 1], XYW[:, :, 0]]
      V_inc[:, :, 0] = \
        delta_t * reshape(map_coordinates(V[:, :, 0], XYW, mode="constant", cval=nan), 
                          V.shape[0:2])
      V_inc[:, :, 1] = \
        delta_t * reshape(map_coordinates(V[:, :, 1], XYW, mode="constant", cval=nan), 
                          V.shape[0:2])
    
    V_tot += coeff * V_inc
  
  XYW = XY + V_tot
  XYW = [XYW[:, :, 1], XYW[:, :, 0]]
  IW = reshape(map_coordinates(I, XYW, mode="constant", cval=nan), I.shape)
  
  return IW
