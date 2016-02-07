
#if defined (WITH_OPENCV) && defined(WITH_CGAL)
#ifndef LUCASKANADEOPENCV_H

#include "SparseMotionExtractor.h"

/// A wrapper class for the OpenCV implementation of the pyramidal Lucas and Kanade algorithm.
class LucasKanadeOpenCV : public SparseMotionExtractor
{
public:
  LucasKanadeOpenCV();
  
  LucasKanadeOpenCV(int numPyramidLevels,
                    int windowSize,
                    int maxNumFeaturePoints,
                    double minFeaturePointDist,
                    double qualityLevel,
                    double stopCritMaxNumIter,
                    double stopCritEpsilon);
  
  void compute(const CImg< unsigned char > &I1,
               const CImg< unsigned char > &I2,
               SparseVectorField &V);
  
  string getName() const;
  
  bool isDual() const;
  
  void printInfoText() const;
private:
  const double EPSILON_;
  const int MAX_NUM_FEATURE_POINTS_;
  const int MAX_NUM_ITER_;
  const double MIN_POINT_DISTANCE_;
  const int NUM_PYRAMID_LEVELS_;
  const double QUALITY_LEVEL_;
  const int WINDOW_SIZE_;
};

#define LUCASKANADEOPENCV_H

#endif
#endif
