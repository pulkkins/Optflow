
#include "DenseImageMorpher.h"
#include "InverseDenseImageExtrapolator.h"

#include "CImg_config.h"
#include <CImg.h>
#include <stdexcept>

namespace DenseImageMorpher
{
  void morph(const CImg< unsigned char > &I1,
             const CImg< unsigned char > &I2,
             const CImg< double > &V1,
             const CImg< double > &V2,
             double t,
             CImg< unsigned char > &M)
  {
    if (t < 0.0 || t > 1.0)
      throw std::invalid_argument("interpolation parameter t must be between 0 and 1");

    CImg< unsigned char > I1e, I2e;
    InverseDenseImageExtrapolator e1, e2;

    e1.extrapolate(I1, V1, t, I1e);
    e2.extrapolate(I2, V2, 1.0 - t, I2e);

    M = (1.0 - t) * I1e + t * I2e;
  }
}