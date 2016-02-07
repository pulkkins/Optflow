
#ifndef PYRAMIDALMOTIONEXTRACTOR_H

#include "ImagePyramid.h"
#include "DenseMotionExtractor.h"

#include <exception>
#include "CImg_config.h"
#include <CImg.h>

using namespace cimg_library;
using namespace std;

/// Defines a base class for dense pyramidal motion extractors.
/**
 * Defines a motion extractor that uses coarse-to-fine approach with 
 * image pyramids. Each lower-resolution motion field is used as an 
 * initial guess for the next level. Each pyramidal motion extractor 
 * implementation should derive this class. This class implements functionality 
 * for both single and dual motion extractors. Each derived class must supply a 
 * single-resolution motion extractor for to be used for each level.
 *
 * The image pyramid scheme used in this implementation is 
 * based on the article:
 *
 * J. Bouguet, Pyramidal Implementation of the Lucas Kanade 
 * Feature Tracker: Description of the Algorithm, Technical 
 * report, OpenCV documents, Intel Corporation, Microprocessor 
 * Research Labs, 2000
 *
 */
class PyramidalDenseMotionExtractor : public DenseMotionExtractor
{
public:
  virtual ~PyramidalDenseMotionExtractor();
  
  /// Computes the motion field from image 1 to image 2.
  /**
   * @param[in] I1 the first source image
   * @param[in] I2 the second source image
   * @param[out] V the computed motion field
   */
  void compute(const CImg< unsigned char > &I1,
               const CImg< unsigned char > &I2,
               CImg< double > &V);
  
  /// Computes both forward and backward motion fields.
  /**
   * @param[in] I1 the first source image
   * @param[in] I2 the second source image
   * @param[out] VF the computed forward motion field (I1->I2)
   * @param[out] VB the computed backward motion field (I2->I1)
   */
  void compute(const CImg< unsigned char > &I1,
               const CImg< unsigned char > &I2,
               CImg< double > &VF,
               CImg< double > &VB);
  
  /// Returns true if the single-resolution motion extractor uses two-directional flows.
  bool isDual() const;
protected:
  const int NUMLEVELS;
  
  // width and height of the base level (original resolution of input images)
  int baseHeight, baseWidth;
  
  // width and height of the current level
  int curLevelH, curLevelW;
  
  // pyramids for the first and second input image
  ImagePyramid imagePyramids[2];
  
  // gradients of the current level
  CImg< double > curLevelG[2];
  
  // input images in the current pyramid level
  CImg< unsigned char > curLevelI[2];
  
  // current forward flow (and backward flow if used)
  CImg< double > curLevelVF;
  CImg< double > curLevelVB;
  
  // the single-resolution motion extractor that is used in each pyramid level
  DenseMotionExtractor *motionExtractor;
  
  // Constructs a pyramidal motion extractor with a given number of levels.
  PyramidalDenseMotionExtractor(int numLevels);
private:
  // computes motion vectors for the current level
  void computeLevel_(int level,
                     CImg< double > &VF,
                     CImg< double > &VB);
  
  // initializes the next motion vector level, i.e. copies the current vectors 
  // to the next level with each vector multiplied by 2 and interpolated 
  // if necessary
  void initializeNextLevel_(CImg< double > &nextLevelVF,
                            CImg< double > &nextLevelVB);
};

#define PYRAMIDALMOTIONEXTRACTOR_H

#endif
