
#ifndef PYRAMIDALHORNSCHUNCK_H

#include "HornSchunck.h"
#include "PyramidalDenseMotionExtractor.h"

/// Implements pyramidal HornSchunck motion extractor.
class PyramidalHornSchunck : public PyramidalDenseMotionExtractor
{
public:
  PyramidalHornSchunck();
  
  PyramidalHornSchunck(int numIterations,
                       double alpha,
                       double relaxCoeff,
                       int numLevels,
                       HornSchunck::BoundaryConditions boundaryConditions);
  
  ~PyramidalHornSchunck();
  
  string getName() const { return "Pyramidal Horn&Schunck"; }
  
  int getNumResultQualityChannels() const { return 1; }
  
  void printInfoText() const;
};

#define PYRAMIDALHORNSCHUNCK_H

#endif

