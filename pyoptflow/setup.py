
from distutils.core import setup, Extension

core = Extension("pyoptflow.core", 
                 include_dirs=["/usr/include/optflow", "/usr/local/include/optflow", "./pyoptflow/core"], 
		 library_dirs=["/usr/lib", "/usr/lib64", "/usr/local/lib"], 
		 sources=["./pyoptflow/core/core.cpp", "./pyoptflow/core/utils.cpp"], 
		 libraries=["boost_python", "optflow"])

setup(name="pyoptflow", 
      version="0.3", 
      description="Python Interface for Optflow", 
      author="Seppo Pulkkinen", 
      author_email="seppo.pulkkinen@fmi.fi", 
      packages=["pyoptflow"], 
      ext_modules=[core])
