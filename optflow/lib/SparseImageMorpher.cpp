
#ifdef WITH_CGAL

#include "SparseImageExtrapolator.h"
#include "SparseImageMorpher.h"

#include "CImg_config.h"
#include <CImg.h>

void SparseImageMorpher::morph(const CImg< unsigned char > &I1,
                               const CImg< unsigned char > &I2,
                               const SparseVectorField &V1,
                               const SparseVectorField &V2,
                               double t,
                               CImg< unsigned char > &M)
{
  CImg< unsigned char > I1e, I2e;
  SparseImageExtrapolator e1, e2;
  
  e1.extrapolate(I1, V1, t, I1e);
  e2.extrapolate(I2, V2, 1.0 - t, I2e);
  
  M = (1.0 - t) * I1e + t * I2e;
}

#endif
