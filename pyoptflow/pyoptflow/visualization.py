"""Plotting of motion fields and other visualization tools."""

from matplotlib.pylab import figure
from numpy import arange, meshgrid, nan, size
from scipy.ndimage import gaussian_filter
try:
  from skimage.measure import find_contours
  skimage_imported = True
except ImportError:
  skimage_imported = False

def plot_contour_overlay(I1, I2, contour_level, minval, maxval):
  """Plot two consecutive images so that a contour of the first image is 
  plotted on top of the second one.
  
  Parameters
  ----------
  I1 : array-like
    The first image whose contour is plotted.
  I2 : array-like
    The second image that is plotted. Must have the same shape as I1.
  contour_level : float
    The level value of the contour to be plotted.
  minval : float
    Minimum image value to plot. Pixels in I2 below this threshold are not 
    plotted.
  maxval : float
    Maximum image value to plot. Values above maxval are set to maxval.
  
  Returns
  -------
  out : matplotlib.figure.Figure
    Handle of the plotted figure.
  """
  if I1.shape != I2.shape:
    raise ValueError("I1 and I2 must have the same shape")
  if not skimage_imported:
    raise ImportError()
  
  contours = find_contours(gaussian_filter(I1, 1), contour_level)
  
  fig = figure()
  ax = fig.gca()
  
  I2 = I2.copy()
  I2[I2 < minval] = nan
  I2[I2 > maxval] = maxval
  
  im = ax.imshow(I2, vmin=minval, vmax=maxval)
  for ct in contours:
    ax.plot(ct[:, 1], ct[:, 0], linewidth=2, linestyle="-", color='k')
  
  fig.colorbar(im)
  
  ax.set_xlim(0, size(I1, 1))
  ax.set_ylim(size(I1, 0), 0)
  ax.set_xticks([])
  ax.set_yticks([])
  
  return fig

def plot_motion_quiver(V, stride=10):
  """Draw a quiver plot from a motion field.
  
  Parameters
  ----------
  V : array-like
    The motion field (a two-dimensional array).
  stride : int
    The gap (number of pixels) between neighbouring motion vectors.
  
  Returns
  -------
  out : matplotlib.figure.Figure
    Handle of the plotted figure.
  """
  fig = figure()
  ax = fig.gca()
  
  V = V.copy()
  V[:, :, 1] = -V[:, :, 1]
  
  X,Y = meshgrid(arange(size(V, 1)), arange(size(V, 0))[::-1])
  
  ax.quiver(X[::stride, ::stride], Y[::stride, ::stride], V[::stride, ::stride, 0], 
            V[::stride, ::stride, 1], units="xy", scale=1.0)
  
  ax.set_xlim(0, size(V, 1))
  ax.set_ylim(0, size(V, 0))
  ax.set_xticks([])
  ax.set_yticks([])
  
  return fig

def plot_motion_field_components(V, sel_comp=["U", "V"], minval=-10.0, 
                                 maxval=10.0, plot_title=None):
  """Plot individual components of a motion field as colormapped images.
  
  Parameters
  ----------
  V : array-like
    The motion field (a two-dimensional array).
  sel_comp : list
    List of selected components. The available options are 'U'=horizontal 
    component, 'V'=vertical component.
  minval : float
    Minimum value to plot.
  maxval : float
    Maximum value to plot.
  plot_title : str
    Title of the plot.
  
  Returns
  -------
  out : list
    Handles of the plotted figures (of type matplotlib.figure.Figure).
  """
  figs = []
  for sc in sel_comp:
    fig = figure()
    ax = fig.gca()
    
    if sc == "U":
      ci = 0
    else:
      ci = 1
    
    im = ax.imshow(V[:, :, ci], vmin=minval, vmax=maxval)
    ax.set_xticks([])
    ax.set_yticks([])
    
    cb = fig.colorbar(im)
    if sc == "U":
      cb.set_label("motion U-component")
    else:
      cb.set_label("motion V-component")
    
    if plot_title != None:
      ax.set_title(plot_title)
    
    figs.append(fig)
  
  return figs

def plot_motion_field_overlay(I, V, minval, maxval, stride=10, 
                              colorbar_label=None, plot_title=None):
  """Plot motion quiver on top of an image.
  
  Parameters
  ----------
  I : array-like
    The image on which the quiver is to be plotted.
  V : array-like
    The motion field (a two-dimensional array). Must have the same shape as I.
  minval : float
    Minimum image value that is plotted.
  maxval : float
    Maximum image value. Values above maxval are set to maxval.
  stride : int
    The gap (number of pixels) between neighbouring motion vectors.
  colorbar_label : str
    Label of the colorbar.
  plot_title : str
    Title of the plot.
  
  Returns
  -------
  out : matplotlib.figure.Figure
    Handle of the plotted figure.
  """
  if len(V.shape) != 3:
    raise ValueError("V must be a three-dimensional array")
  if I.shape != V.shape[0:2]:
    raise ValueError("I and V must have the same shape")
  
  fig = figure()
  ax = fig.gca()
  
  I = I.copy()
  I[I < minval] = nan
  I[I > maxval] = maxval
  
  X,Y = meshgrid(arange(size(I, 1)), -arange(size(I, 0)))
  
  im = ax.imshow(I, extent=(0, size(I, 1), -size(I, 0)+1, 0), vmin=minval, 
                 vmax=maxval)
  ax.quiver(X[::stride, ::stride], Y[::stride, ::stride], 
            V[::stride, ::stride, 0], -V[::stride, ::stride, 1], 
            color='r', units="xy", scale=1.0)
  cb = fig.colorbar(im)
  if colorbar_label != None:
    cb.set_label(colorbar_label)
  ax.set_xticks([])
  ax.set_yticks([])
  ax.set_xlim(0, size(I, 1))
  ax.set_ylim(-size(I, 0), 0)
  
  if plot_title != None:
    ax.set_title(plot_title)
  
  return fig

def plot_motion_field_quality(V, qc, minval, maxval, plot_title=None):
  """Plot quality maps associated with the given motion field.
  
  Parameters
  ----------
  V : array-like
    The motion field V(x,y,q), where x and y denote spatial coordinates and 
    q>=3 denotes the quality channel (the first two are u and v-components of 
    motion).
  qc : int
    Index of the quality channel to plot.
  plot_title : str
    Title of the plot.
  
  Returns
  -------
  out : matplotlib.figure.Figure
    Handle of the plotted figure.
  """
  if len(V.shape) != 3:
    raise ValueError("V must be a three-dimensional array")
  if V.shape[2] <= 2:
    raise ValueError("V does not contain any quality channels")
  if 2+qc > V.shape[2]:
    raise ValueError("invalid quality channel index")
  
  fig = figure()
  ax = fig.gca()
  
  im = ax.imshow(V[:, :, 2+qc], vmin=minval, vmax=maxval)
  ax.set_xticks([])
  ax.set_yticks([])
  
  cb = fig.colorbar(im)
  cb.set_label("motion quality channel %d" % (qc+1))
  
  if plot_title != None:
    ax.set_title(plot_title)
  
  return fig
