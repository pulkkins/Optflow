"""Implementations of optical flow algorithms."""

from numpy import arange, dstack, hstack, imag, real, unravel_index, zeros
from numpy.linalg import lstsq
from numpy.fft import fftn, ifft2, ifftshift
import sys

def extract_motion_darts(Z, N_x=30, N_y=30, N_t=4, M_x=1, M_y=1, 
                         print_messages=False):
  """Compute the advection field from a sequence of input images by using the 
  DARTS method. The method is described in
  
  E. Ruzanski, V. Chandrasekar and Y. Wang, 2011: The CASA Nowcasting System, 
  Journal of Atmospheric and Oceanic Technology, 28(5), 640-655
  
  Parameters
  ----------
  Z : array-like
    Array of shape (m,n,t) containing a sequence of t two-dimensional input 
    images of shape (m,n).
  N_x : int
    Number of Fourier series terms to use for the input images, x-axis.
  N_y : int
    Number of Fourier series terms to use for the input images, y-axis.
  N_t : int
    Number of Fourier series terms to use for the input images, time axis.
  M_x : int
    Number of Fourier series terms to compute for the output advection field, 
    x-axis.
  M_y : int
    Number of Fourier series terms to compute for the output advection field, 
    y-axis.
  print_messages : bool
    If True, print information messages.
  
  Returns
  -------
  out : numpy.ndarray
    An array of shape (m,n,2) containing the x- and y-components of the estimated 
    motion vectors.
  """
  T_x = Z.shape[1]
  T_y = Z.shape[0]
  T_t = Z.shape[2]
  
  if print_messages:
    print("Computing the FFT of the reflectivity fields..."),
    sys.stdout.flush()
  
  F = fftn(Z)
  
  if print_messages:
    print("Done.")
    
    print("Constructing the y-vector..."),
    sys.stdout.flush()
  
  m = (2*N_x+1)*(2*N_y+1)*(2*N_t+1)
  n = (2*M_x+1)*(2*M_y+1)
  
  y = zeros(m, dtype=complex)
  
  k_y,k_x,k_t = unravel_index(arange(m), (2*N_y+1, 2*N_x+1, 2*N_t+1))
  
  for i in xrange(m):
    k_x_ = k_x[i] - N_x
    k_y_ = k_y[i] - N_y
    k_t_ = k_t[i] - N_t
    
    F_ = F[k_y_, k_x_, k_t_]
    
    y[i] = k_t_ * F_
  
  if print_messages:
    print("Done.")
  
  A = zeros((m, n), dtype=complex)
  B = zeros((m, n), dtype=complex)
  
  if print_messages:
    print("Constructing the H-matrix..."),
    sys.stdout.flush()
  
  c1 = -1.0*T_t / (T_x * T_y)
  
  k_y,k_x,k_t = unravel_index(arange(m), (2*N_y+1, 2*N_x+1, 2*N_t+1))
  kp_y,kp_x = unravel_index(arange(n), (2*M_y+1, 2*M_x+1))
  
  for i in xrange(m):
    k_x_  = k_x[i] - N_x
    k_y_  = k_y[i] - N_y
    k_t_  = k_t[i] - N_t
    
    kp_x_ = kp_x[:] - M_x
    kp_y_ = kp_y[:] - M_y
    
    i_ = k_y_ - kp_y_
    j_ = k_x_ - kp_x_
    
    F_ = F[i_, j_, k_t_]
    
    c2 = c1 / T_y * i_
    A[i, :] = c2 * F_
    
    c2 = c1 / T_x * j_
    B[i, :] = c2 * F_
  
  if print_messages:
    print("Done.")
  
    print("Solving the linear systems..."),
    sys.stdout.flush()
  
  x = lstsq(hstack([A, B]), y)[0]
  
  if print_messages:
    print("Done.")
    
    print("Computing the velocity field from the FFT coefficients..."),
    sys.stdout.flush()
  
  h,w = 2*M_y+1,2*M_x+1
  
  U = zeros((h, w), dtype=complex)
  V = zeros((h, w), dtype=complex)
  
  i,j = unravel_index(arange(h*w), (h, w))
  
  def fill(X, h, w):
    X_f = zeros((h, w), dtype=complex)
    cy = int(h / 2)
    cx = int(w / 2)
    dy = int(X.shape[0] / 2)
    dx = int(X.shape[1] / 2)
    X_f[cy-dy:cy+dy+1, cx-dx:cx+dx+1] = X
    X_f = ifftshift(X_f)
    
    return X_f
  
  V[i, j] = x[0:h*w]
  V = fill(V, Z.shape[0], Z.shape[1])
  V = real(ifft2(V))
  
  U[i, j] = x[h*w:2*h*w]
  U = fill(U, Z.shape[0], Z.shape[1])
  U = real(ifft2(U))
  
  if print_messages:
    print("Done.")
  
  return dstack([U, V])
