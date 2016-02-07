
#include "DualDenseMotionExtractor.h"
#include "PyramidalDenseMotionExtractor.h"

#include <stdexcept>

PyramidalDenseMotionExtractor::~PyramidalDenseMotionExtractor() { }

void PyramidalDenseMotionExtractor::compute(const CImg< unsigned char > &I1,
                                            const CImg< unsigned char > &I2,
                                            CImg< double > &V)
{
  CImg< double > VB; // not used
  compute(I1, I2, V, VB);
}

void PyramidalDenseMotionExtractor::compute(const CImg< unsigned char > &I1,
                                            const CImg< unsigned char > &I2,
                                            CImg< double > &VF,
                                            CImg< double > &VB)
{
  const int W = I1.width();
  const int H = I1.height();
  
  CImg< double > nextLevelVF;
  CImg< double > nextLevelVB;
  
  // Check that the input images have the same dimensions;
  if(I1.width() != I2.width() || I1.height() != I2.height())
    throw invalid_argument("The dimensions of the input images must match.");
  
  imagePyramids[0] = ImagePyramid(I1, NUMLEVELS);
  imagePyramids[1] = ImagePyramid(I2, NUMLEVELS);
  
  if(VF.width() != W || VF.height() != H || VF.spectrum() != getNumResultChannels())
    VF = CImg< double >(W, H, 1, 2 + getNumResultQualityChannels());
  if(isDual())
  {
    if(VB.width() != W || VB.height() != H || VB.spectrum() != getNumResultChannels())
      VB = CImg< double >(W, H, 1, 2 + getNumResultQualityChannels());
  }
  
  baseWidth = W;
  baseHeight = H;
  
  printInfoText();
  
  curLevelW = imagePyramids[0].getImageLevel(NUMLEVELS - 1).width();
  curLevelH = imagePyramids[0].getImageLevel(NUMLEVELS - 1).height();
  
  curLevelVF = CImg< double >(curLevelW, curLevelH, 1, getNumResultChannels());
  curLevelVF.fill(0);
  if(isDual())
  {
    curLevelVB = CImg< double >(curLevelW, curLevelH, 1, getNumResultChannels());
    curLevelVB.fill(0);
  }
  
  for(int i = NUMLEVELS - 1; i >= 0; i--)
  {
    computeLevel_(i, curLevelVF, curLevelVB);
    
    if(i > 0)
    {
      curLevelW = imagePyramids[0].getImageLevel(i-1).width();
      curLevelH = imagePyramids[0].getImageLevel(i-1).height();
      
      nextLevelVF = CImg< double >(curLevelW, curLevelH, 1, getNumResultChannels());
      nextLevelVF.fill(0);
      if(isDual())
      {
        nextLevelVB = CImg< double >(curLevelW, curLevelH, 1, getNumResultChannels());
	nextLevelVB.fill(0);
      }
      
      initializeNextLevel_(nextLevelVF, nextLevelVB);
      
      curLevelVF = nextLevelVF;
      if(isDual())
        curLevelVB = nextLevelVB;
    }
  }
  
  VF = curLevelVF;
  if(isDual())
    VB = curLevelVB;
}

bool PyramidalDenseMotionExtractor::isDual() const
{
  return motionExtractor->isDual();
}

PyramidalDenseMotionExtractor::PyramidalDenseMotionExtractor(int numLevels) : 
  NUMLEVELS(numLevels)
{ }

void PyramidalDenseMotionExtractor::computeLevel_(int level,
                                                  CImg< double > &VF,
                                                  CImg< double > &VB)
{
  curLevelI[0].assign(imagePyramids[0].getImageLevel(level), true);
  curLevelI[1].assign(imagePyramids[1].getImageLevel(level), true);
  
  if(isDual())
    dynamic_cast< DualDenseMotionExtractor * >(motionExtractor)->
      compute(curLevelI[0], curLevelI[1], VF, VB);
  else
    motionExtractor->compute(curLevelI[0], curLevelI[1], VF);
}

void PyramidalDenseMotionExtractor::initializeNextLevel_(CImg< double > &nextLevelVF,
                                                         CImg< double > &nextLevelVB)
{
  const int W_NEW = nextLevelVF.width();
  const int H_NEW = nextLevelVF.height();
  
  double vxf, vyf;
  double vxb, vyb;
  
  double xc, yc;
  int xn, yn;
  
  for(yn = 0; yn < H_NEW; yn++)
  {
    yc = ((double)yn) / 2.0;
    for(xn = 0; xn < W_NEW; xn++)
    {
      xc = ((double)xn) / 2.0;
      if(xn % 2 != 0 || yn % 2 != 0)
      {
        vxf = curLevelVF.linear_atXY(xc, yc, 0, 0);
        vyf = curLevelVF.linear_atXY(xc, yc, 0, 1);
        if(isDual())
        {
          vxb = curLevelVB.linear_atXY(xc, yc, 0, 0);
          vyb = curLevelVB.linear_atXY(xc, yc, 0, 1);
        }
      }
      else
      {
        vxf = curLevelVF(xc, yc, 0, 0);
        vyf = curLevelVF(xc, yc, 0, 1);
        if(isDual())
        {
          vxb = curLevelVB(xc, yc, 0, 0);
          vyb = curLevelVB(xc, yc, 0, 1);
        }
      }
      
      nextLevelVF(xn, yn, 0, 0) = 2.0 * vxf;
      nextLevelVF(xn, yn, 0, 1) = 2.0 * vyf;
      if(isDual())
      {
        nextLevelVB(xn, yn, 0, 0) = 2.0 * vxb;
        nextLevelVB(xn, yn, 0, 1) = 2.0 * vyb;
      }
    }
  }
}
