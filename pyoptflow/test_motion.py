# A simple test script for computing and plotting a motion field between two 
# precipitation fields.

from pylab import *
import h5py
from pyoptflow import io_utils, utils, visualization
from pyoptflow.core import extract_motion_proesmans

# Read precipitation fields from HDF5 files (in the ODIM format).
I1 = h5py.File("precipfield1.h5", 'r')["dataset1"]["data1"]["data"][...]
I2 = h5py.File("precipfield2.h5", 'r')["dataset1"]["data1"]["data"][...]

# Threshold out near-zero precipitation intensities for plotting.
I1_ = I1.copy()
I1_[I1 < 0.05] = nan
I2_ = I2.copy()
I2_[I2 < 0.05] = nan

# Plot the original precipitation fields.
figure()
imshow(I1_, vmin=0.05, vmax=10)
cb = colorbar()
cb.set_label("precipitation intensity (mm/h)")
xticks([])
yticks([])
savefig("input1.png", bbox_inches="tight")

figure()
imshow(I2_, vmin=0.05, vmax=10)
cb = colorbar()
cb.set_label("precipitation intensity (mm/h)")
xticks([])
yticks([])
savefig("input2.png", bbox_inches="tight")

# Contour plot of the first input image on top of the second one.
visualization.plot_contour_overlay(I1, I2, 0.05, 0.05, 10.0)
savefig("contour_overlay.png", bbox_inches="tight")

# Convert the precipitation fields to unsigned byte, as required by the Optflow 
# motion estimation algorithms. Gaussian filter with std. dev. 3 is applied.
I1 = utils.rainfall_to_ubyte(I1, R_min=0.05, R_max=10.0, filter_stddev=3.0)
I2 = utils.rainfall_to_ubyte(I2, R_min=0.05, R_max=10.0, filter_stddev=3.0)

# Compute the motion field by using the Python <-> C++ API and the Proesmans 
# algorithm.
V = extract_motion_proesmans(I1, I2, lam=25.0, num_iter=250, num_levels=6)[0]

# Plot the U- and V- (horizontal and vertical) components of the motion field.
figs = visualization.plot_motion_field_components(V, sel_comp=["U", "V"])
figs[0].savefig("motion_U.png", bbox_inches="tight")
figs[1].savefig("motion_V.png", bbox_inches="tight")

# Plot the quality map of the motion field.
visualization.plot_motion_field_quality(V, 0, 0.0, 1.0)
savefig("motion_quality.png", bbox_inches="tight")

# Quiver plot of the motion field.
visualization.plot_motion_quiver(V, stride=15)
savefig("motion_quiver.png", bbox_inches="tight")

# Quiver plot on top of the input image.
visualization.plot_motion_field_overlay(I1_, V, 0.05, 10.0, stride=15, 
                                        colorbar_label="precipitation intensity (mm/h)")
savefig("motion_overlay.png", bbox_inches="tight", dpi=200)

io_utils.write_ODIM_HDF5(V, "motion.h5", ignore_missing_attrs=True, 
                         write_as_image=True)
