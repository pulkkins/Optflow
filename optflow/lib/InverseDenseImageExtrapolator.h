
#ifndef INVERSEDENSEIMAGEEXTRAPOLATOR_H

#include "DenseImageExtrapolator.h"

/// Implements inverse-mapped image extrapolation with dense motion fields.
/**
 * For each pixel in the destination image, the method implemented in this 
 * class finds the corresponding pixel in the source image. This is done by 
 * using an inverse motion field between the source images.
 */
class InverseDenseImageExtrapolator : public DenseImageExtrapolator
{
public:
  void extrapolate(const CImg< unsigned char > &I0, 
                   const CImg< double > &V,
                   double multiplier,
                   CImg< unsigned char > &Ie) const;
};

#define INVERSEDENSEIMAGEEXTRAPOLATOR_H

#endif
