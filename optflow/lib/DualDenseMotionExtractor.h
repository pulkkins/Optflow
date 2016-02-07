
#ifndef DUALDENSEMOTIONEXTRACTOR_H

#include "DenseMotionExtractor.h"

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;

/// Defines the interface of motion extractors for extracting dual and dense motion fields.
/**
 * This class defines the interface for extracting dual and dense motion fields. 
 * A dual motion field consists of the forward motion field (I1->I2) and the 
 * backward motion field (I2->I1).
 */
class DualDenseMotionExtractor : public DenseMotionExtractor
{
 public:
  virtual ~DualDenseMotionExtractor() { }
  
  void compute(const CImg< unsigned char > &I1,
               const CImg< unsigned char > &I2,
               CImg< double > &V);
  
  /// Extracts the dual motion field between two source images.
  /**
   * Extracts forward and backward motion between two 8-bit grayscale source images.
   * @param[in] I1 The first source image.
   * @param[in] I2 The second source image.
   * @param[out] VF The computed forward motion vector field.
   * @param[out] VB The computed backward motion vector field.
   */
  virtual void compute(const CImg< unsigned char > &I1,
                       const CImg< unsigned char > &I2,
                       CImg< double > &VF,
                       CImg< double > &VB) = 0;
};

#define DUALDENSEMOTIONEXTRACTOR_H

#endif
