
#include "utils.hpp"

namespace utils
{ 
  array make_array(int m, int n, int d, double fill)
  {
    list l;
    for(int i = 0; i < m * n * d; i++)
      l.append(fill);
    tuple t = tuple(l);
    array a(t);
    array a_ = (array)a.copy();
    a_.resize(make_tuple(m, n, d));
    
    return a_;
  }
}
