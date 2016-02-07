 
#include "LucasKanadeROI.h"

LucasKanadeROI::LucasKanadeROI(int x, int y, int w, int h, const CImg< unsigned char > &I_, const CImg< double > &G_):
  ROI(x, y, w, h, NULL), G(G_), I(I_)
{
}

LucasKanadeROI::LucasKanadeROI(int x, int y, int w, int h, const CImg< unsigned char > &I_, const CImg< double > &G_, const CImg< double > *W):
  ROI(x, y, w, h, W), G(G_), I(I_)
{
}

void LucasKanadeROI::addNewTerm(int x, int y)
{
  double gxs = G(x, y, 0, 0);
  double gys = G(x, y, 0, 1);
  int I1s = I(x, y);
  double w;
  
  if(W_ != NULL)
    w = (*W_)(x - anchorx_, y - anchory_);
  else
    w = 1.0;
  
  GWG[0][0] += w * gxs * gxs;
  GWG[0][1] += w * gxs * gys;
  GWG[1][0] += w * gys * gxs;
  GWG[1][1] += w * gys * gys;
	
  /*rhsI1Sum[0] += I1s * gxs;
    rhsI1Sum[1] += I1s * gys;*/
}

double LucasKanadeROI::computeDeterminant() const
{
  return GWG[0][0] * GWG[1][1] - GWG[1][0] * GWG[0][1];
}

double LucasKanadeROI::getGWGElement(int i, int j) const
{
  return GWG[i][j];
}

/*double LucasKanadeROI::getRHSTerm(int i)
  {
  return rhsI1Sum[i];
  }*/

void LucasKanadeROI::initialize()
{
  const int IW = G.width();
  const int IH = G.height();
  
  int i, j;
  int wx, wy;
  double gxs, gys;
  double w;
  int I1s;
  
  //coordHandler = CoordinateHandler< int >(0, 0, IW - 1, IH - 1);
  
  GWG[0][0] = 0.0;
  GWG[0][1] = 0.0;
  GWG[1][0] = 0.0;
  GWG[1][1] = 0.0;
	
  //rhsI1Sum[0] = rhsI1Sum[1] = 0.0;
	
  for(i = 0; i < height_; i++)
  {
    for(j = 0; j < width_; j++)
    {
      wx = anchorx_ + j; 
      wy = anchory_ + i;
      //coordHandler.clamp(wx, wy, wx, wy);
      /*if(!coordHandler.isInside(wx, wy))
        continue;*/
      
      gxs = G.linear_atXY(wx, wy, 0, 0);
      gys = G.linear_atXY(wx, wy, 0, 1);
      
      I1s = I.linear_atXY(wx, wy);
      
      if(W_ != NULL)
        w = (*W_)(j, i);
      else
        w = 1.0;
      
      GWG[0][0] += w * gxs * gxs;
      GWG[0][1] += w * gxs * gys;
      GWG[1][0] += w * gxs * gys;
      GWG[1][1] += w * gys * gys;
      
      /*rhsI1Sum[0] += w * I1s * gxs;
        rhsI1Sum[1] += w * I1s * gys;*/
    }
  }
}

void LucasKanadeROI::subtractOldTerm(int x, int y)
{
  double gxs = G(x, y, 0, 0);
  double gys = G(x, y, 0, 1);
  int I1s = I(x, y);
  double w;
  
  if(W_ != NULL)
    w = (*W_)(x - anchorx_, y - anchory_);
  else
    w = 1.0;
  
  GWG[0][0] -= w * gxs * gxs;
  GWG[0][1] -= w * gxs * gys;
  GWG[1][0] -= w * gys * gxs;
  GWG[1][1] -= w * gys * gys;
  
  /*rhsI1Sum[0] -= I1s * gxs;
    rhsI1Sum[1] -= I1s * gys;*/
}
