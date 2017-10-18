import numpy
from distutils.core import setup, Extension

# The external algorithms that will be compiled with pyoptflow. Please see the 
# README file of each subdirectory in pyoptflow/ext to obtain the required files.
with_brox     = False
with_brox2    = False
with_brox2_st = False
with_clg      = False

ext_include_dirs = []
ext_library_dirs = []
ext_sources      = []
ext_libraries    = []

if with_brox == True:
  ext_include_dirs.append("./pyoptflow/ext/brox")
  ext_library_dirs.append("./pyoptflow/ext/brox")
  ext_libraries.append("of")

if with_brox2 == True:
  ext_include_dirs.append("./pyoptflow/ext/brox2")

if with_brox2_st == True:
  ext_include_dirs.append("./pyoptflow/ext/brox2_st")

if with_clg == True:
  ext_include_dirs.append("./pyoptflow/ext/clg")
  ext_sources.append("./pyoptflow/ext/clg/bicubic_interpolation.c")
  ext_sources.append("./pyoptflow/ext/clg/clg_of.c")
  ext_sources.append("./pyoptflow/ext/clg/iio.c")
  ext_sources.append("./pyoptflow/ext/clg/mask.c")
  ext_sources.append("./pyoptflow/ext/clg/util.c")
  ext_sources.append("./pyoptflow/ext/clg/zoom.c")

include_dirs = ["/usr/include/optflow", 
                "/usr/local/include/optflow", 
                "./pyoptflow/core",
                numpy.get_include()]
include_dirs.extend(ext_include_dirs)

library_dirs = ["/usr/lib", 
                "/usr/lib64", 
                "/usr/local/lib"]
library_dirs.extend(ext_library_dirs)

sources = ["./pyoptflow/core/core.cpp", "./pyoptflow/core/utils.cpp"]
sources.extend(ext_sources)

libraries = ["boost_python", "optflow", "tiff", "png"]
libraries.extend(ext_libraries)

define_macros = []
if with_brox == True:
  define_macros.append(("WITH_BROX", 1))
if with_brox2 == True:
  define_macros.append(("WITH_BROX2", 1))
if with_brox2_st == True:
  define_macros.append(("WITH_BROX2_ST", 1))
if with_clg == True:
  define_macros.append(("WITH_CLG", 1))

core = Extension("pyoptflow.core", 
                 include_dirs=include_dirs, 
                 library_dirs=library_dirs, 
                 sources=sources, 
                 libraries=libraries, 
                 extra_compile_args=["-std=c99"], 
                 define_macros=define_macros)

setup(name="pyoptflow", 
      version="0.4", 
      description="Python Interface for Optflow", 
      author="Seppo Pulkkinen", 
      author_email="seppo.pulkkinen@fmi.fi", 
      packages=["pyoptflow"], 
      ext_modules=[core])
