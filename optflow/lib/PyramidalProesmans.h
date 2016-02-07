
#ifndef PYRAMIDALPROESMANS_H

#include "Proesmans.h"
#include "PyramidalDenseMotionExtractor.h"

/// Implements pyramidal Proesmans motion extractor.
class PyramidalProesmans : public PyramidalDenseMotionExtractor
{
public:
  PyramidalProesmans();
  
  PyramidalProesmans(int numIterations_,
                     float lambda_,
                     int numLevels,
                     Proesmans::BoundaryConditions boundaryConditions_);
  
  ~PyramidalProesmans();
  
  string getName() const { return "Pyramidal Proesmans"; }
  
  int getNumResultQualityChannels() const { return 1; }
  
  void printInfoText() const;
};

#define PYRAMIDALPROESMANS_H

#endif
