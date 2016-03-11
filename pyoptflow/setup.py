
from distutils.core import setup, Extension

with_brox = True
with_clg  = True

ext_include_dirs = []
ext_library_dirs = []
ext_sources      = []
ext_libraries    = []

if with_brox == True:
  ext_include_dirs.append("./pyoptflow/ext/brox")
  ext_library_dirs.append("./pyoptflow/ext/brox")
  ext_libraries.append("of")

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
                "./pyoptflow/core"]
include_dirs.extend(ext_include_dirs)

library_dirs = ["/usr/lib", 
                "/usr/lib64", 
                "/usr/local/lib"]
library_dirs.extend(ext_library_dirs)

sources = ["./pyoptflow/core/core.cpp", "./pyoptflow/core/utils.cpp"]
sources.extend(ext_sources)

libraries = ["boost_python", "optflow"] #, "tiff", "png"
libraries.extend(ext_libraries)

core = Extension("pyoptflow.core", 
                 include_dirs=include_dirs, 
                 library_dirs=library_dirs, 
                 sources=sources, 
                 libraries=libraries, 
                 extra_compile_args=["-std=c99"], 
                 define_macros=[("WITH_BROX", 1), ("WITH_CLG", 1)]) # TODO: Make these optional.

setup(name="pyoptflow", 
      version="0.3", 
      description="Python Interface for Optflow", 
      author="Seppo Pulkkinen", 
      author_email="seppo.pulkkinen@fmi.fi", 
      packages=["pyoptflow"], 
      ext_modules=[core])
