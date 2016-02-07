
#ifdef WITH_CGAL
#ifndef SPARSEMOTIONEXTRACTOR_H

#include "SparseVectorField.h"

#include <list>
#include <vector>

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;
using namespace std;

/// Defines the interface for extracting sparse motion vector fields. 
/**
 * This class defines the interface for sparse motion extractors. 
 * A sparse vector field consists of a set of "feature points" that 
 * describe the moving objects in an image. 
 * A motion vector is identified by a difference of feature point 
 * positions between two images.
 */
class SparseMotionExtractor
{
public:
  /// Extracts a sparse vector field between two source images.
  /**
   * @param[in] I1 first source image
   * @param[in] I2 second source image
   * @param[in] featurePoints1 feature points on the first image
   * @param[in] featurePoints2 transformed feature points on the second image
   * @param[in] triangulate generate triangulation of the extracted feature points (optional)
   * @param[out] triIndices indices for each triangle vertex, each index refers to the feature point array
   */
  virtual void compute(const CImg< unsigned char > &I1,
                       const CImg< unsigned char > &I2,
                       SparseVectorField &V) = 0;
  
  /// Prints information about the motion extractor and its parameters.
  virtual void printInfoText() const = 0;
};

#define SPARSEMOTIONEXTRACTOR_H

#endif
#endif
