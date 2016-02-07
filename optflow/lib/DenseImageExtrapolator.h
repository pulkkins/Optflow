
#ifndef DENSEIMAGEEXTRAPOLATOR_H

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;

/// Abstract base class for warping images with dense motion fields.
class DenseImageExtrapolator
{
public:
  /// Warps the given image by using a dense motion vector field.
  /**
   * @param[in] I0 the image to warp
   * @param[in] V the vector field to use
   * @param[in] t time step multiplier
   * @param[out] Ie warped image
   */
  virtual void extrapolate(const CImg< unsigned char > &I0,
                           const CImg< double > &V,
                           double multiplier,
                           CImg< unsigned char > &Ie) const = 0;
};

#define DENSEIMAGEEXTRAPOLATOR_H

#endif
