
#ifndef DENSEMOTIONEXTRACTOR_H

#include <stdexcept>

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;

/// Defines the interface for dense motion extractors.
/**
 * This class defines the interface for dense motion extractors. 
 * In this context dense vector field means a vector field that 
 * has a vector assigned at each integer coordinate. 
 */
class DenseMotionExtractor
{
 public:
  virtual ~DenseMotionExtractor() { }
    
  /// Extracts motion between two source images.
  /**
   * Extracts motion between two 8-bit grayscale source images.
   * @param[in] I1 the first source image
   * @param[in] I2 the second source image
   * @param[out] V the computed motion vector field
   */
  virtual void compute(const CImg< unsigned char > &I1,
                       const CImg< unsigned char > &I2,
                       CImg< double > &V) = 0;
  
  /// Extracts motion between two source images.
  /**
   * Extracts forward and backward motion fields between two 8-bit grayscale 
   * source images (if the backward motion can be computed).
   * @param[in] I1 the first source image
   * @param[in] I2 the second source image
   * @param[out] VF the computed forward motion vector field
   * @param[out] VB the computed backward motion vector field
   */
  virtual void compute(const CImg< unsigned char > &I1,
                       const CImg< unsigned char > &I2,
                       CImg< double > &VF,
                       CImg< double > &VB)
  {
    throw std::runtime_error("Motion extractor does not support computing dual motion fields.");
  }
  
  /// Returns the number of channels in the resulting motion vector field.
  /**
   * The number of channels is 2+number of quality channels (x,y,q1,...).
   */
  int getNumResultChannels() const { return 2 + getNumResultQualityChannels(); }
  
  /// Returns the number of quality channels in the resulting motion vector field.
  virtual int getNumResultQualityChannels() const = 0;
  
  /// Returns true if this motion extractor is dual, i.e. it produces two-directional flows.
  virtual bool isDual() const = 0;
  
  /// Prints information about the motion extractor and its parameters.
  virtual void printInfoText() const = 0;
};

#define DENSEMOTIONEXTRACTOR_H

#endif
