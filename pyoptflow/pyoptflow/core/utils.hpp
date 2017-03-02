
#include <boost/python.hpp>
#include <numpy/ndarrayobject.h>

#include "CImg_config.h"
#include <CImg.h>

using namespace boost::python;
using namespace cimg_library;

namespace utils
{
  numeric::array make_array(int m, int n, int d, double fill);
  
  struct CImg_to_ndarray
  {
    static PyObject* convert(const CImg< double > &I)
    {
      numeric::array I_ndarray = make_array(I.height(), I.width(), I.spectrum(), 0.0);
      
      for(int y = 0; y < I.height(); y++)
        for(int x = 0; x < I.width(); x++)
          for(int c = 0; c < I.spectrum(); c++)
            I_ndarray[boost::python::make_tuple(y, x, c)] = I(x, y, 0, c);
      
      return incref(I_ndarray.ptr());
    }
  };
  
  struct ndarray_to_CImg
  {
    ndarray_to_CImg()
    {
      boost::python::converter::registry::push_back(&convertible, &construct, 
        boost::python::type_id< CImg< unsigned char > >());
    }
    
    static void *convertible(PyObject *obj_ptr)
    {
      if(!PyArray_Check(obj_ptr))
        return NULL;
      PyArray_Descr *dtype = PyArray_DTYPE((PyArrayObject *)obj_ptr);
      if(dtype->type != 'B' || dtype->kind != 'u')
        return NULL;
      
      return obj_ptr;
    }
    
    static void construct(PyObject *obj_ptr, 
      boost::python::converter::rvalue_from_python_stage1_data *data)
    {
      const int m = PyArray_DIM(obj_ptr, 0);
      const int n = PyArray_DIM(obj_ptr, 1);
      
      void *storage = (
        (boost::python::converter::rvalue_from_python_storage< CImg< unsigned char > > *)
        data)->storage.bytes;
      new (storage) CImg< unsigned char >(n, m);
      CImg< unsigned char > *I = (CImg< unsigned char > *)storage;
      unsigned char *array_data = (unsigned char *)PyArray_DATA(obj_ptr);
      
      for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
          (*I)(j, i) = array_data[j + i*n];
      data->convertible = storage;
    }
  };
}
