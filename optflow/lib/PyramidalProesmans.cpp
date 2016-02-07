
#include <iostream>

#include "PyramidalProesmans.h"

PyramidalProesmans::PyramidalProesmans() : PyramidalDenseMotionExtractor(4)
{
  motionExtractor = new Proesmans();
}

PyramidalProesmans::PyramidalProesmans(int numIterations,
                                       float lambda,
                                       int numLevels,
                                       Proesmans::BoundaryConditions boundaryConditions) : 
  PyramidalDenseMotionExtractor(numLevels)
{
  motionExtractor = new Proesmans(numIterations, lambda, boundaryConditions);
}

PyramidalProesmans::~PyramidalProesmans()
{
  delete motionExtractor;
}

void PyramidalProesmans::printInfoText() const
{
  Proesmans *me = dynamic_cast< Proesmans * >(motionExtractor);
  
  cout<<"Pyramidal Proesmans optical flow algorithm"<<endl;
  cout<<"=========================================="<<endl;
  
  cout<<"Input image width: "<<baseWidth<<endl;
  cout<<"Input image height: "<<baseHeight<<endl;
  
  cout<<"Number of iteration steps: "<<me->getNumIterations()<<endl;
  cout<<"Lambda: "<<me->getLambda()<<endl;
  cout<<"Number of pyramid levels: "<<NUMLEVELS<<endl;
  cout<<"Boundary conditions: ";
  if(me->getBoundaryConditions() == Proesmans::NEUMANN)
    cout<<"Neumann"<<endl;
  else
    cout<<"Dirichlet"<<endl;
}
