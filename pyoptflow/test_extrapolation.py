# A simple test script for semi-Lagrangian extrapolation of a rainfall map.

from pylab import *
import h5py
from pyoptflow import utils
from pyoptflow.core import extract_motion_proesmans
from pyoptflow.extrapolation import semilagrangian

# Read rainfall maps from HDF5 files (in the ODIM format).
I1 = h5py.File("rainfallmap1.h5", 'r')["dataset1"]["data1"]["data"][...]
I2 = h5py.File("rainfallmap2.h5", 'r')["dataset1"]["data1"]["data"][...]

# Threshold out near-zero rainfall intensities.
I1_ = I1.copy()
I1_[I1 < 0.05] = nan
I2_ = I2.copy()
I2_[I2 < 0.05] = nan

# Plot the original rainfall maps.
figure()
imshow(I1_, vmin=0.05, vmax=10)
cb = colorbar()
cb.set_label("rain rate (mm/h)")
xticks([])
yticks([])
savefig("input1.png", bbox_inches="tight")

figure()
imshow(I2_, vmin=0.05, vmax=10)
cb = colorbar()
cb.set_label("rain rate (mm/h)")
xticks([])
yticks([])
savefig("input2.png", bbox_inches="tight")

# Convert the rainfall maps to unsigned byte, as required by the Optflow 
# motion detection algorithms. Gaussian filter with std. dev. 3 is applied.
I1_ubyte = utils.rainfall_to_ubyte(I1, R_min=0.05, R_max=10.0, filter_stddev=3.0)
I2_ubyte = utils.rainfall_to_ubyte(I2, R_min=0.05, R_max=10.0, filter_stddev=3.0)

# Compute the motion field by using the Python <-> C++ API and the Proesmans 
# algorithm.
V = extract_motion_proesmans(I1_ubyte, I2_ubyte, lam=25.0, num_iter=250, 
                             num_levels=6)[0]

# Extrapolate the first input image five time steps.
for t in arange(1, 6):
  I_extrap = semilagrangian(I1, V, t, 15, n_iter=3, inverse=True)
  figure()
  I = I_extrap.copy()
  I[I < 0.05] = nan
  imshow(I, vmin=0.05, vmax=10)
  cb = colorbar()
  cb.set_label("rain rate (mm/h)")
  xticks([])
  yticks([])
  savefig("input_extrap_%d.png" % t, bbox_inches="tight")
  close()
