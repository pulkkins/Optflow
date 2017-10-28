Overview
--------

Optflow is a C++ library for motion detection and motion-based extrapolation 
and morphing algorithms.

The following motion detection algorithms are implemented in Optflow:

  * the Horn&Schunck variational approach
  * global anisotropic diffusion algorithm by Proesmans et. al.
  * Lucas&Kanade least-squares -based block matching
  * interface for the Lucas&Kanade algorithm implemented in OpenCV

The first two algorithms generate dense vector fields (i.e. motion vectors for 
each pixel in the source image). On the other hand, the Lucas&Kanade -based 
algorithms generate sparse motion fields. This is done by computing the motion 
for a given set of "feature points". All the above algorithms are implemented by 
using image pyramids.

Optflow also contains algorithms for image warping by using a motion field. The 
following methods are implemented:

  * Inverse mapping: or each pixel in the second input image, find the 
    corresponding pixel in the first one.
  * Forward mesh extrapolation: construct a textured triangle mesh from the 
    image and transform the mesh by using the motion field.

Dependencies and installation
-----------------------------

Required dependencies:

    CImg       >= 1.5       http://cimg.sourceforge.net

Optional dependencies:

    Boost.Program_options   http://www.boost.org
    CGAL       >= 4.2       http://www.cgal.org
    OpenCV     >= 2.4       http://sourceforge.net/projects/opencv

Optflow uses CMake for generating the makefiles. To build and install the 
package, create a build directory, and type the following commands in it:

    cmake <src_dic> -DCMAKE_INSTALL_PREFIX=<dest_dir>
    make
    sudo make install

The last step is optional. You can also use the compiled packages from the build 
directory. The src_dir argument points to the source directory, and dest_dir 
is the installation directory (e.g. /usr or /usr/local). Enabling and disabling 
external dependencies can be done via the following options:

    -DWITH_BOOST_PROGRAM_OPTIONS=ON/OFF  command-line interface via Boost.Program_options
    -DWITH_CGAL=ON/OFF                   support for sparse motion fields via CGAL
    -DWITH_OPENCV=ON/OFF                 support for OpenCV algorithms

The installation is done to the following subdirectories in the destination 
directory:

    bin                binaries
    lib                libraries (liboptflow.so)
    include/optflow    headers

The compiled package includes three binaries: `extractmotion`, `extrapolate` and 
`morph`. To print their command-line syntax, run them without arguments. The 
file `liboptflow.so` contains a linkable library.

Experimental support for MATLAB is also implemented. A MEX-file and a MATLAB 
script for running it are located in the matlab-directory.

References
----------

The motion detection algorithms in this package are based on the following 
references:

[1] J. Bouguet, Pyramidal Implementation of the Lucas Kanade 
    Feature Tracker: Description of the Algorithm, Technical 
    report, OpenCV documents, Intel Corporation, Microprocessor 
    Research Labs, 2000

[2] B.K. Horn and B. Schunck, Determining Optical Flow, Artificial Intelligence, 
    vol. 17, pp. 185-203, 1981

[3] B.D. Lucas and T. Kanade, An iterative image registration 
    technique with an application to stereo vision, in Proc. 
    Seventh International Joint Conference on Artificial 
    Intelligence, Vancouver, 1981, pp. 674-679

[4] M. Proesmans, L. Van Gool, E. Pauwels, and A. Oosterlinck, 
    Determination of optical ﬂow and its discontinuities using 
    non-linear diffusion, in 3rd European Conference on Computer 
    Vision, ECCV’94, 1994, Vol. 2, pp. 295–304.

and the C-based implementations found at:

ftp://ftp.csd.uwo.ca/pub/vision  
http://of-eval.sourceforge.net

Author: Seppo Pulkkinen <seppo.pulkkinen@fmi.fi>
