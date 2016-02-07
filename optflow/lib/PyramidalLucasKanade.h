
#ifndef PYRAMIDALLUCASKANADE_H

#include "PyramidalDenseMotionExtractor.h"

#include <list>

using namespace std;

/// Implements pyramidal Lucas and Kanade motion extractor.
/**
 * Implements the Lucas and Kanade algorithm. This is 
 * based on the implementation by Galvin and Mccane. 
 * In addition, this implementation uses image 
 * pyramids and an arbitrary number of Gauss-Newton iterations. 
 */
class PyramidalLucasKanade : public PyramidalDenseMotionExtractor
{
public:
  PyramidalLucasKanade();
  
  /// Constructs a Lucas and Kanade motion extractor with the given parameters.
  /**
   * @param windowRadius_ Radius of the "observation" window. Window size is 2*radius+1.
   * @param numIter_ Number of Gauss-Newton iterations.
   * @param tau_ Eigenvalue threshold parameter.
   * @param sigmap_ Regularization parameter.
   * @param numLevels_ Number of pyramid levels.
   */
  PyramidalLucasKanade(int windowRadius,
                       int numIter,
                       float tau,
                       float sigmap,
                       int numLevels,
                       bool useWeightingKernel);
  
  ~PyramidalLucasKanade();
  
  string getName() const { return "Pyramidal Lucas-Kanade"; }
  
  int getNumResultQualityChannels() const { return 2; }
  
  void printInfoText() const;
};

#define PYRAMIDALLUCASKANADE_H

#endif
