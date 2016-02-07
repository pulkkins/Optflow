
#ifndef FORWARDDENSEIMAGEEXTRAPOLATOR_H

#include "DenseImageExtrapolator.h"

// TODO: This is not implemented.

/// Implements forward image extrapolation with dense motion fields.
/**
 * Forward image extrapolation by computing the coordinates of each pixel in 
 * the second image by starting from the first one and using the computed 
 * motion field between two images. All pixels in the destination image do not 
 * necessarily get a value. Hence, additional postprocessing is needed for 
 * filling the "holes".
 */
class ForwardDenseImageExtrapolator : public DenseImageExtrapolator
{
public:
  void extrapolate(const CImg< unsigned char > &I0,
                   const CImg< double > &V,
                   double t,
                   CImg< unsigned char > &Ie) const;
};

#define FORWARDDENSEIMAGEEXTRAPOLATOR_H

#endif
