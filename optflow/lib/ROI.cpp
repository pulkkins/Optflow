
#include <cstdlib>
#include <cmath>

#include "ROI.h"

ROI::ROI() { }

ROI::ROI(int x, int y, int w, int h,
         const CImg< double > *W) : 
         anchorx_(x), anchory_(y), width_(w), height_(h), W_(W)
{ }

void ROI::translate(int dx, int dy)
{
  if(dx == 0 && dy == 0)
    return;
  
  // revert to slower mode if a weighting kernel is used
  if(W_ != NULL)
  {
    anchorx_ += dx;
    anchory_ += dy;
    initialize();
    return;
  }
  
  // TODO: Test that this function works properly. When used in LucasKanade, 
  // this throws a segfault...
  
  if(abs(dx) >= width_ || abs(dy) >= height_)
  {
    anchorx_ += dx;
    anchory_ += dy;
    initialize();
    return;
  }
  
  int i, j;
  int il, ir, jt, jb;
  int x, y;
  
  // Subtract old values.
  jt = dy > 0 ? 0:height_ + dy;
  jb = dy > 0 ? dy:height_;
  
  for(j = jt; j < jb; j++)
  {
    for(i = 0; i < width_; i++)
    {
      x = anchorx_ + i;
      y = anchory_ + j;
      
      //coordHandler.clamp(x, y, x, y);
      //if(coordHandler.isInside(x, y))
      subtractOldTerm(x, y);
    }
  }
  
  il = dx > 0 ? 0:width_ + dx;
  ir = dx > 0 ? dx:width_;
  jt = dy > 0 ? dy:0;
  jb = dy > 0 ? height_:height_ + dy;
  
  for(j = jt; j < jb; j++)
  {
    for(i = il; i < ir; i++)
    {
      x = anchorx_ + i;
      y = anchory_ + j;
      
      //coordHandler.clamp(x, y, x, y);
      //if(coordHandler.isInside(x, y))
      subtractOldTerm(x, y);
    }
  }
  
  // Add new values.
  jt = dy > 0 ? height_:dy;
  jb = dy > 0 ? dy:0;
  
  for(j = jt; j < jb; j++)
  {
    for(i = 0; i < width_; i++)
    {
      x = anchorx_ + i;
      y = anchory_ + j;
      
      //coordHandler.clamp(x, y, x, y);
      //if(coordHandler.isInside(x, y))
      addNewTerm(x, y);
    }
  }

  il = dx > 0 ? width_:0;
  ir = dx > 0 ? width_ + dx:dx;
  jt = dy > 0 ? dy:0;
  jb = dy > 0 ? height_:height_ + dy;

  for(j = jt; j < jb; j++)
  {
    for(i = il; i < ir; i++)
    {
      x = anchorx_ + i;
      y = anchory_ + j;
      
      //coordHandler.clamp(x, y, x, y);
      //if(coordHandler.isInside(x, y))
      addNewTerm(x, y);
    }
  }

  anchorx_ += dx;
  anchory_ += dy;
}
