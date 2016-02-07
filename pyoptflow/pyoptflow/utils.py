"""Conversion functions for weather radar and rainfall data."""

from numpy import isfinite, log, ubyte
from scipy.ndimage import gaussian_filter

def dBZ_to_ubyte(I, dBZ_min=-10.0, dBZ_max=50.0, filter_stddev=3.0):
  """Convert a dBZ field into a 8-bit image, as required by Optflow. Optionally, 
  apply a Gaussian smoothing filter.
  
  Parameters
  ----------
  I : array-like
    The dBZ field.
  dBZ_min : float
    Minimum dBZ. Values smaller than dBZ_min are set to dBZ_min. If None, 
    dBZ_min is computed from I.
  dBZ_max : float
    Maximum dBZ. Values greater than dBZ_max are set to dBZ_max. If None, 
    dBZ_max is computed from I.
  filter_stddev : float
    Standard deviation of the Gaussian filter (0=no filtering)
  
  Returns
  -------
  out : ndarray(dtype=ubyte)
    The processed dBZ field.
  """
  I = I.copy()
  MASK = isfinite(I)
  
  if dBZ_min == None:
    dBZ_min = min(I[MASK])
  if dBZ_max == None:
    dBZ_max = max(I[MASK])
  
  I[~MASK] = dBZ_min
  I[I < dBZ_min] = dBZ_min
  I[I > dBZ_max] = dBZ_max
  
  if filter_stddev > 0.0:
    I = gaussian_filter(I, filter_stddev, mode="nearest")
  
  I = ((I - dBZ_min) / (dBZ_max - dBZ_min)) * 255.0
  
  return I.astype(ubyte)

def rainfall_to_ubyte(I, R_min=0.1, R_max=40.0, filter_stddev=3.0):
  """Convert a rainfall intensity field into a 8-bit image, as required by 
  Optflow. Optionally, apply a Gaussian smoothing filter.
  
  Parameters
  ----------
  I : array-like
    The rainfall field.
  R_min : float
    Minimum rainfall intensity. Values smaller than R_min are set to R_min. 
    If None, R_min is computed from I.
  R_max : float
    Maximum rainfall intensity. Values greater than R_max are set to R_max. 
    If None, R_max is computed from I.
  filter_stddev : float
    Standard deviation of the Gaussian filter (0=no filtering)
  
  Returns
  -------
  out : ndarray(dtype=ubyte)
    The processed rainfall field.
  """
  I = I.copy()
  MASK = isfinite(I)
  
  if R_min == None:
    R_min = min(I[MASK])
  if R_max == None:
    R_max = max(I[MASK])
  
  I[~MASK] = R_min
  I[I < R_min] = R_min
  I[I > R_max] = R_max
  I = log(I)
  
  R_min = log(R_min)
  R_max = log(R_max)
  
  if filter_stddev > 0.0:
    I = gaussian_filter(I, filter_stddev, mode="nearest")
  
  I = ((I - R_min) / (R_max - R_min)) * 255.0
  
  return I.astype(ubyte)
