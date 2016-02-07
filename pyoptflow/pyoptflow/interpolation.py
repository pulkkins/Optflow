"""Methods for temporal interpolation between images by using a motion vector 
field."""

from numpy import arange, dstack, meshgrid, reshape, size
from scipy.ndimage.interpolation import map_coordinates

def interpolate(I1, I2, V, n):
  """Interpolate n frames between two images by using a motion vector field. 
  The time range between the images is equally divided into n subintervals.
  
  Parameters
  ----------
  I1 : array-like
    The first input image.
  I2 : array-like
    The second input image (must have the same shape as I1).
  V : array-like
    The motion vector field (must have the same shape as I1 and I2).
  
  Returns
  -------
  out : list
    List of length n containing the interpolated frames between the images. 
    The input images are not included in the list.
  """
  if len(I1.shape) != 2:
    raise ValueError("I1 and I2 must be two-dimensional arrays")
  if I1.shape != I2.shape:
    raise ValueError("I1 and I2 must have the same shape")
  if V.shape[0:2] != I1.shape or V.shape[0:2] != I2.shape:
    raise ValueError("V must have the same shape as I1 and I2")
  
  X,Y = meshgrid(arange(size(I1, 1)), arange(size(I1, 0)))
  XY = dstack([X, Y])
  
  tws = 1.0*arange(1, n+1) / (n + 1)
  
  I_result = []
  for tw in tws:
    XYW = XY - tw*V[:,:,0:2]
    XYW = [XYW[:, :, 1], XYW[:, :, 0]]
    I1_warped = reshape(map_coordinates(I1, XYW), I1.shape)
    XYW = XY + (1.0-tw)*V[:,:,0:2]
    XYW = [XYW[:, :, 1], XYW[:, :, 0]]
    I2_warped = reshape(map_coordinates(I2, XYW), I2.shape)
    
    I_result.append((1.0-tw)*I1_warped + tw*I2_warped)
  
  return I_result
