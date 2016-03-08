
#include <boost/python.hpp>
#include "CImg_config.h"
#include <CImg.h>
#include <numpy/ndarrayobject.h>
#include "PyramidalHornSchunck.h"
#include "PyramidalLucasKanade.h"
#include "PyramidalProesmans.h"
#include "utils.hpp"

using namespace boost::python;
using namespace boost::python::numeric;
using namespace cimg_library;
using namespace utils;

CImg< double > extract_motion_hornschunck(const CImg< unsigned char > &I1, 
                                          const CImg< unsigned char > &I2, 
                                          int num_iter, 
                                          float alpha, 
                                          float relaxcoeff, 
                                          int num_levels)
{
  PyramidalHornSchunck me(num_iter, alpha, relaxcoeff, num_levels, 
			  HornSchunck::NEUMANN);
  CImg< double > V;
  me.compute(I1, I2, V);
  
  return V;
}

CImg< double > extract_motion_lucaskanade(const CImg< unsigned char > &I1, 
                                          const CImg< unsigned char > &I2, 
                                          int window_radius, 
                                          int num_iter, 
                                          float tau, 
                                          float sigmap, 
                                          int num_levels, 
                                          bool use_weights)
{
  PyramidalLucasKanade me(window_radius, num_iter, tau, sigmap, num_levels, 
                          use_weights);
  CImg< double > V;
  me.compute(I1, I2, V);
  
  return V;
}

tuple extract_motion_proesmans(const CImg< unsigned char > &I1, 
                               const CImg< unsigned char > &I2, 
                               float lam, 
                               int num_iter, 
                               int num_levels)
{
  PyramidalProesmans me(num_iter, lam, num_levels, Proesmans::NEUMANN);
  CImg< double > VF, VB;
  me.compute(I1, I2, VF, VB);
  
  return make_tuple(VF, VB);
}

BOOST_PYTHON_MODULE(core)
{
  array::set_module_and_type("numpy", "ndarray");
  to_python_converter< CImg< double >, CImg_to_ndarray >();
  import_array();
  ndarray_to_CImg();
  
  def("extract_motion_hornschunck", &extract_motion_hornschunck, 
      (boost::python::arg("num_iter")=500, 
       boost::python::arg("alpha")=0.7, 
       boost::python::arg("relaxcoeff")=1.9, 
       boost::python::arg("num_levels")=4));
  
  def("extract_motion_lucaskanade", &extract_motion_lucaskanade, 
      (boost::python::arg("window_radius")=16, 
       boost::python::arg("num_iter")=5, 
       boost::python::arg("tau")=0.0025f, 
       boost::python::arg("sigmap")=0.0f, 
       boost::python::arg("num_levels")=4, 
       boost::python::arg("use_weights")=false));
  
  def("extract_motion_proesmans", &extract_motion_proesmans, 
      (boost::python::arg("lam")=100.0f, 
       boost::python::arg("num_iter")=200, 
       boost::python::arg("num_levels")=4));
}
