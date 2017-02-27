
#include "utils.hpp"

namespace utils
{ 
  numeric::array make_array(int m, int n, int d, double fill)
  {
    list l;
    for(int i = 0; i < m * n * d; i++)
      l.append(fill);
    boost::python::tuple t = boost::python::tuple(l);
    numeric::array a(t);
    numeric::array a_ = (numeric::array)a.copy();
    a_.resize(make_tuple(m, n, d));
    
    return a_;
  }
}
