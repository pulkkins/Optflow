
#include <iostream>

#include "HornSchunck.h" 
#include "PyramidalHornSchunck.h" 

PyramidalHornSchunck::PyramidalHornSchunck() : PyramidalDenseMotionExtractor(4)
{
  motionExtractor = new HornSchunck();
}

PyramidalHornSchunck::PyramidalHornSchunck(int numIterations,
                                           double alpha,
                                           double relaxCoeff,
                                           int numLevels,
                                           HornSchunck::BoundaryConditions boundaryConditions) : 
  PyramidalDenseMotionExtractor(numLevels)
{
  motionExtractor = new HornSchunck(numIterations, alpha, relaxCoeff, boundaryConditions);
}

PyramidalHornSchunck::~PyramidalHornSchunck()
{
  delete motionExtractor;
}

void PyramidalHornSchunck::printInfoText() const
{
  HornSchunck *me = dynamic_cast< HornSchunck * >(motionExtractor);
  
  cout<<"Horn&Schunck optical flow algorithm"<<endl;
  cout<<"==================================="<<endl;
  
  cout<<"Input image width: "<<baseWidth<<endl;
  cout<<"Input image height: "<<baseHeight<<endl;
  
  cout<<"Number of iterations: "<<me->getNumIterations()<<endl;
  cout<<"Alpha: "<<me->getAlpha()<<endl;
  cout<<"Relaxation coefficient: "<<me->getRelaxCoeff()<<endl;
  cout<<"Number of pyramid levels: "<<NUMLEVELS<<endl;
  cout<<"Boundary conditions: ";
  if(me->getBoundaryConditions() == HornSchunck::NEUMANN)
    cout<<"Neumann"<<endl;
  else
    cout<<"Dirichlet"<<endl;
}
