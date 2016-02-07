
#ifndef DENSEIMAGEMORPHER_H

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;

/// Computes an intermediate image between two images.
/**
 * This class implements a morphing algorithm that computes an interpolated 
 * image between two images. Before cross-fading, the algorithm warps the 
 * first and second images by using the forward and inverse motion fields, 
 * respectively. This algorithm uses dense motion vector fields.
 */
namespace DenseImageMorpher
{
  /// Computes the intermediate image between two images at the given time.
  /**
   * @param[in] I1 the first source image
   * @param[in] I2 the second source image
   * @param[in] V1 the forward motion field (I1->I2)
   * @param[in] V2 the inverse motion field (I2->I1)
   * @param[in] t interpolation coefficient (0<=t<=1)
   * @param[out] M the resulting image
   */
  void morph(const CImg< unsigned char > &I1,
             const CImg< unsigned char > &I2,
             const CImg< double > &V1,
             const CImg< double > &V2,
             double t,
             CImg< unsigned char > &M);
}

#define DENSEIMAGEMORPHER_H

#endif
