"""Conversion functions for weather radar and rainfall data."""

from numpy import isfinite, log, ubyte
from scipy.ndimage import gaussian_filter

def dBZ_to_ubyte(I, dBZ_min=-10.0, dBZ_max=50.0, dBZ_rmin=None, filter_stddev=0.0):
  """Rescale and convert a reflectivity field in dBZ units into a 8-bit image, 
  as required by Optflow. Optionally, apply a Gaussian smoothing filter.
  
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
  dBZ_rmin : float
    If given, reflectivity values below dBZ_min are set to dBZ_rmin. Defaults to 
    dBZ_min.
  filter_stddev : float
    Standard deviation of the Gaussian filter (0=no filtering)
  
  Returns
  -------
  out : ndarray(dtype=ubyte)
    The converted reflectivity field scaled to the range [dBZ_rmin,dBZ_max].
  """
  if dBZ_max <= dBZ_min:
    raise ValueError("dBZ_max must be greater than dBZ_min")
  
  I = I.copy()
  MASK = isfinite(I)
  
  if dBZ_min == None:
    dBZ_min = min(I[MASK])
  if dBZ_max == None:
    dBZ_max = max(I[MASK])
  
  if dBZ_rmin == None:
    dBZ_rmin = dBZ_min
  elif dBZ_rmin > dBZ_min:
    raise ValueError("dBZ_rmin must not be greater than dBZ_min")
  
  I[~MASK] = dBZ_rmin
  I[I < dBZ_min] = dBZ_rmin
  I[I > dBZ_max] = dBZ_max
  
  if filter_stddev > 0.0:
    I = gaussian_filter(I, filter_stddev, mode="reflect")
  
  I = (I - dBZ_rmin) / (dBZ_max - dBZ_rmin) * 255.0
  
  return I.astype(ubyte)

def rainfall_to_ubyte(R, R_min=0.1, R_max=40.0, R_rmin=None, filter_stddev=0.0, 
                      logtrans=False):
  """Rescale and convert a rainfall intensity field into a 8-bit image, as 
  required by Optflow. Optionally, apply a Gaussian smoothing filter.
  
  Parameters
  ----------
  R : array-like
    The input rainfall field.
  R_min : float
    Minimum rainfall intensity. Values smaller than R_min are set to R_min. 
    If None, R_min is computed from R.
  R_max : float
    Maximum rainfall intensity. Values greater than R_max are set to R_max. 
    If None, R_max is computed from R.
  R_rmin : float
    If given, rainfall intensity values below R_min are set to R_rmin. Defaults 
    to R_min.
  filter_stddev : float
    Standard deviation of the Gaussian filter (0=no filtering)
  logtrans : bool
    If True, apply a log-transform to the input rainfall field. In this case, 
    R_min must be nonzero.
  
  Returns
  -------
  out : ndarray(dtype=ubyte)
    The converted rainfall field scaled to the range [R_rmin,R_max].
  """
  if R_max <= R_min:
    raise ValueError("R_max must be greater than R_min")
  
  R = R.copy()
  MASK = isfinite(R)
  
  if R_min == None:
    R_min = min(R[MASK])
  if R_max == None:
    R_max = max(R[MASK])
  
  if R_rmin == None:
    R_rmin = R_min - (R_max - R_min)
  elif R_rmin > R_min:
    raise ValueError("R_rmin must not be greater than R_min")
  
  R[~MASK] = R_rmin
  R[R < R_min] = R_rmin
  R[R > R_max] = R_max
  
  if logtrans == True:
    if R_min <= 0.0:
      raise ValueError("R_min must be positive if log-transform is used")
    R = log(R)
    R_min = log(R_min)
    R_max = log(R_max)
  
  R = (R - R_rmin) / (R_max - R_rmin) * 255.0
  
  R = R.astype(ubyte)
  
  if filter_stddev > 0.0:
    R = gaussian_filter(R, filter_stddev, mode="reflect")
  
  return R
