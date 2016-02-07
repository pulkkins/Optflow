
#include <iostream>

#include "LucasKanade.h"
#include "PyramidalLucasKanade.h"

PyramidalLucasKanade::PyramidalLucasKanade():PyramidalDenseMotionExtractor(4)
{
  motionExtractor = new LucasKanade();
}

PyramidalLucasKanade::PyramidalLucasKanade(int windowRadius,
                                           int numIter,
                                           float tau,
                                           float sigmap,
                                           int numLevels,
                                           bool useWeightingKernel) : 
  PyramidalDenseMotionExtractor(numLevels)
{
  motionExtractor = new LucasKanade(windowRadius, numIter, tau, sigmap, useWeightingKernel);
}

PyramidalLucasKanade::~PyramidalLucasKanade()
{
  delete motionExtractor;
}

void PyramidalLucasKanade::printInfoText() const
{
  LucasKanade *me = dynamic_cast< LucasKanade * >(motionExtractor);
	
  cout<<"Pyramidal Lucas & Kanade optical flow algorithm"<<endl;
  cout<<"==============================================="<<endl;
		
  cout<<"Input image width: "<<baseWidth<<endl;
  cout<<"Input image height: "<<baseHeight<<endl;
	
  cout<<"Window size: "<<me->getWindowSize()<<endl;
  cout<<"Number of Gauss-Newton iterations: "<<me->getNumIterations()<<endl;
  cout<<"Tau (eigenvalue threshold): "<<me->getTau()<<endl;
  cout<<"Sigmap (regularization parameter): "<<me->getSigmap()<<endl;
  cout<<"Number of pyramid levels: "<<NUMLEVELS<<endl;
}
