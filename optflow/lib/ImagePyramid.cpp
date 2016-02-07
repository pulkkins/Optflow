
#include "ImagePyramid.h"

#include "CImg_config.h"
#include <CImg.h>

ImagePyramid::ImagePyramid() { }

ImagePyramid::ImagePyramid(const CImg< unsigned char > &I0, int n)
{
  int w = I0.width();
  int h = I0.height();
  CImg< unsigned char > currentLevel;
  CImg< unsigned char > nextLevel;
  
  levels_.push_back(I0);
  currentLevel = I0;
  for(int l = 1; l < n; l++)
  {
    nextLevel = CImg< unsigned char >(w / 2, h / 2);
    computeNextLevel_(currentLevel, nextLevel);
    currentLevel = nextLevel;
    
    levels_.push_back(currentLevel);
    
    w /= 2;
    h /= 2;
  }
}

const CImg< unsigned char > &ImagePyramid::getImageLevel(int i) const
{
  return levels_[i];
}

int ImagePyramid::getNumLevels() const
{
  return levels_.size();
}

void ImagePyramid::computeNextLevel_(const CImg< unsigned char > &src,
                                     CImg< unsigned char > &dest)
{
  const int DW = dest.width();
  const int DH = dest.height();
  
  for(int y = 0; y < DH; y++)
  {
    for(int x = 0; x < DW; x++)
    {
      dest(x, y) = (src(2*x, 2*y)   + src(2*x+1, 2*y) + 
                    src(2*x, 2*y+1) + src(2*x+1, 2*y+1)) / 4;
    }
  }
}
