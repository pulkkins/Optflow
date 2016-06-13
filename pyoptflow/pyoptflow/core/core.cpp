
#include <boost/python.hpp>
#include "CImg_config.h"
#include <CImg.h>
#include <numpy/ndarrayobject.h>
#include "PyramidalHornSchunck.h"
#include "PyramidalLucasKanade.h"
#include "PyramidalProesmans.h"
#include "utils.hpp"

#ifdef WITH_BROX
#include "../ext/brox/of.h"
#endif

#ifdef WITH_BROX2
#include "../ext/brox2/brox_optic_flow.h"
#endif

#ifdef WITH_BROX_ST
#include "../ext/brox_st/brox_optic_flow.h"
#endif

#ifdef WITH_CLG
extern "C" {
#include "../ext/clg/clg_of.h"
}
#endif

#include <iostream>

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

#ifdef WITH_BROX
CImg< double > extract_motion_brox(const CImg< unsigned char > &I1, 
                                   const CImg< unsigned char > &I2, 
                                   float sigma, 
                                   float alpha, 
                                   float gamma)
{
  int w = I1.width();
  int h = I1.height();
  int x, y, i;
  
  CTensor<float> img1(w, h, 1);
  CTensor<float> img2(w, h, 1);
  
  for (x = 0; x < w; x++)
  {
    for (y = 0; y < h; y++)
    {
      img1(x, y, 0) = I1(x, y);
      img2(x, y, 0) = I2(x, y);
    }
  }
  
  CTensor<float> flow;
  opticalFlow(img1, img2, flow, sigma, alpha, gamma);
  
  CImg< double > V(w, h, 1, 2);
  for (x = 0; x < w; x++)
    for (y = 0; y < h; y++)
      for (i = 0; i < 2; i++)
        V(x, y, 0, i) = flow(x, y, i);
  
  return V;
}
#endif

#ifdef WITH_BROX2
CImg< double > extract_motion_brox2(const CImg< unsigned char > &I1, 
                                    const CImg< unsigned char > &I2, 
                                    float alpha, 
                                    float gamma, 
                                    int nscales, 
                                    float nu, 
                                    float tol, 
                                    int inner_iter, 
                                    int outer_iter)
{
  int w = I1.width();
  int h = I1.height();
  int x, y, i;
  
  float *I1_arr = new float[w*h];
  float *I2_arr = new float[w*h];
  float *u = new float[w*h];
  float *v = new float[w*h];
  
  for (x = 0; x < w; x++)
  {
    for (y = 0; y < h; y++)
    {
      I1_arr[y*w + x] = I1(x, y);
      I2_arr[y*w + x] = I2(x, y);
    }
  }
  
  brox_optic_flow(I1_arr, I2_arr, u, v, w, h, alpha, gamma, nscales, nu, tol, 
                  inner_iter, outer_iter, 0);
  
  CImg< double > V(w, h, 1, 2);
  for (x = 0; x < w; x++)
  {
    for (y = 0; y < h; y++)
    {
      V(x, y, 0, 0) = u[y*w + x];
      V(x, y, 0, 1) = v[y*w + x];
    }
  }
  
  delete[] I1_arr;
  delete[] I2_arr;
  delete[] u;
  delete[] v;
  
  return V;
}
#endif

#ifdef WITH_BROX_ST
/*CImg< double > extract_motion_brox_st(const tuple &I, float alpha, float gamma, 
  int nscales, float nu, float tol, int inner_iter, int outer_iter)*/
tuple extract_motion_brox_st(const tuple &I, float alpha, float gamma, 
  int nscales, float nu, float tol, int inner_iter, int outer_iter)
{
  const int nframes = len(I);
  int i;
  array I0_arr = extract< array >(I[0]);
  tuple ts = extract< tuple >(I0_arr.attr("shape"));
  const int ny = extract< int >(ts[0]);
  const int nx = extract< int >(ts[1]);
  int x, y;
  
  float *I_arr = new float[nx*ny*nframes];
  float *u = new float[nx*ny*(nframes-1)];
  float *v = new float[nx*ny*(nframes-1)];
  
  for (i = 0; i < nframes; i++)
  {
    array I_i = extract< array >(I[i]);
    PyObject *obj_ptr = I_i.ptr();
    unsigned char *I_data = (unsigned char *)PyArray_DATA(obj_ptr);
	  for (x = 0; x < nx; x++)
      for (y = 0; y < ny; y++)  
        I_arr[i*nx*ny + y*nx + x] = I_data[y*nx + x];
  }
  
  //brox_optic_flow(I_arr, u, v, nx, ny, nframes, 75.0, 7.0, 10, 0.75, 1e-4, 1, 15, 0);
  brox_optic_flow(I_arr, u, v, nx, ny, nframes, alpha, gamma, nscales, nu, tol, 
                  inner_iter, outer_iter, 0);
  
  array U_ = make_array(ny, nx, nframes-1, 0.0);
  array V_ = make_array(ny, nx, nframes-1, 0.0);
  for (i = 0; i < nframes-1; i++)
  {
    for (x = 0; x < nx; x++)
    {
      for (y = 0; y < ny; y++)
      {
        U_[make_tuple(y, x, i)] = u[(nframes-2)*nx*ny + y*nx + x];
        V_[make_tuple(y, x, i)] = v[(nframes-2)*nx*ny + y*nx + x];
      }
    }
  }
  
  tuple UV = make_tuple(U_, V_);
  
  /*CImg< double > V(nx, ny, 1, 2);
  for (x = 0; x < nx; x++)
  {
    for (y = 0; y < ny; y++)
    {
      V(x, y, 0, 0) = u[(nframes-2)*nx*ny + y*nx + x];
      V(x, y, 0, 1) = v[(nframes-2)*nx*ny + y*nx + x];
    }
  }*/
  
  delete[] I_arr;
  delete[] u;
  delete[] v;
  
  return UV;
}
#endif

#ifdef WITH_CLG
CImg< double > extract_motion_clg(const CImg< unsigned char > &I1, 
                                  const CImg< unsigned char > &I2, 
                                  int iterations, 
                                  double alpha, 
                                  double rho,
                                  double sigma, 
                                  double wfactor,
                                  int nscales)
{
  int w = I1.width();
  int h = I1.height();
  int x, y;
  
  double *image1, *image2;
  double *uOut, *vOut;
  
  image1 = new double[w*h];
  image2 = new double[w*h];
  uOut   = new double[w*h];
  vOut   = new double[w*h];
  
  for (y = 0; y < h; y++)
  {
    for (x = 0; x < w; x++)
    {
      image1[x+y*w] = I1(x, y);
      image2[x+y*w] = I2(x, y);
    }
  }
  
  calcMSCLG_OF(image1, image2, uOut, vOut, h, w, iterations, alpha, rho, sigma, 
               wfactor, nscales, 0.65, 1, 0);
  
  delete[] image1;
  delete[] image2;
  
  CImg< double > V(w, h, 1, 2);
  
  for (y = 0; y < h; y++)
  {
    for (x = 0; x < w; x++)
    {
      V(x, y, 0, 0) = uOut[x+y*w];
      V(x, y, 0, 1) = vOut[x+y*w];
    }
  }
  
  delete[] uOut;
  delete[] vOut;
  
  return V;
}
#endif

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
  
  #ifdef WITH_BROX
  def("extract_motion_brox", &extract_motion_brox, 
      (boost::python::arg("sigma")=0.8f, 
       boost::python::arg("alpha")=80.0f, 
       boost::python::arg("gamma")=5.0f));
  #endif
  
  #ifdef WITH_BROX2
  def("extract_motion_brox2", &extract_motion_brox2, 
      (boost::python::arg("alpha")=50.0f, 
       boost::python::arg("gamma")=5.0f, 
       boost::python::arg("nscales")=10, 
       boost::python::arg("nu")=0.75f, 
       boost::python::arg("tol")=1e-4, 
       boost::python::arg("inner_iter")=1, 
       boost::python::arg("outer_iter")=50));
  #endif
 
  #ifdef WITH_BROX_ST
  def("extract_motion_brox_st", &extract_motion_brox_st, 
      (boost::python::arg("alpha")=18.0f, 
       boost::python::arg("gamma")=7.0f, 
       boost::python::arg("nscales")=100, 
       boost::python::arg("nu")=0.75, 
       boost::python::arg("tol")=0.0001, 
       boost::python::arg("inner_iter")=1, 
       boost::python::arg("outer_iter")=15));
  #endif
  
  #ifdef WITH_CLG
  def("extract_motion_clg", &extract_motion_clg, 
      (boost::python::arg("iterations")=1000, 
       boost::python::arg("alpha")=200.0, 
       boost::python::arg("rho")=5.0, 
       boost::python::arg("sigma")=0.85, 
       boost::python::arg("wfactor")=1.8, 
       boost::python::arg("nscales")=4));
  #endif
}
