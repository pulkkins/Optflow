
#include "DualDenseMotionExtractor.h"

#include "CImg_config.h"
#include <CImg.h>

void DualDenseMotionExtractor::compute(const CImg< unsigned char > &I1,
                                       const CImg< unsigned char > &I2,
                                       CImg< double > &V)
{
  CImg< double > VB;
  
  return compute(I1, I2, V, VB);
}
