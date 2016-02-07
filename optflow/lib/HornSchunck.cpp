
#include <iostream>

#include "HornSchunck.h"

HornSchunck::HornSchunck() : BOUNDARY_CONDITIONS_(NEUMANN),
                             INTENSITY_SCALE_(1.0 / 255.0),
                             ALPHA_(75.0),
                             RELAX_COEFF_(1.95),
                             NUM_ITERATIONS_(200)
{ }

HornSchunck::HornSchunck(int numIterations_,
                         double alpha_,
                         double relaxCoeff_,
                         BoundaryConditions boundaryConditions_) : 
  BOUNDARY_CONDITIONS_(boundaryConditions_),
  INTENSITY_SCALE_(1.0 / 255.0),
  ALPHA_(alpha_),
  RELAX_COEFF_(relaxCoeff_),
  NUM_ITERATIONS_(numIterations_)
{ }

void HornSchunck::compute(const CImg< unsigned char > &I1,
                          const CImg< unsigned char > &I2,
                          CImg< double > &V)
{
  int x, y, i;
  double uAvg, vAvg;
  double numer, denom;
  
  width_  = I1.width();
  height_ = I1.height();
  
  CImg< double > V_;
  V_.assign(V, true);
  
  computeGradients_(I1, I2);
  
  for(i = 0; i < NUM_ITERATIONS_; i++)
  {
    for(y = 1; y < height_ - 1; y++)
    {
      //#pragma omp parallel for ordered schedule(static) shared(V_) private(numer,denom,uAvg,vAvg)
      for(x = 1; x < width_ - 1; x++)
      {
        uAvg = (V_(x, y-1, 0)   + V_(x+1, y, 0) + 
                V_(x, y+1, 0)   + V_(x-1, y, 0)) / 6.0 + 
               (V_(x-1, y-1, 0) + V_(x+1, y-1, 0) + 
                V_(x-1, y+1, 0) + V_(x+1, y+1, 0)) / 12.0;
        
        vAvg = (V_(x, y-1, 1)   + V_(x+1, y, 1) + 
                V_(x, y+1, 1)   + V_(x-1, y, 1)) / 6.0 + 
               (V_(x-1, y-1, 1) + V_(x+1, y-1, 1) + 
                V_(x-1, y+1, 1) + V_(x+1, y+1, 1)) / 12.0;
        
        numer = Gx_(x, y)*uAvg + Gy_(x, y)*vAvg + Gt_(x, y);
        denom = ALPHA_*ALPHA_ + Gx_(x, y)*Gx_(x, y) + Gy_(x, y)*Gy_(x, y);
        
        //#pragma omp ordered
        V_(x, y, 0) = (1.0 - RELAX_COEFF_) * V_(x, y, 0) + 
                      RELAX_COEFF_ * (uAvg - Gx_(x, y)*numer/denom);
        V_(x, y, 1) = (1.0 - RELAX_COEFF_) * V_(x, y, 1) + 
                      RELAX_COEFF_ * (vAvg - Gy_(x, y)*numer/denom);
        V_(x, y, 2) = 1.0;
      }
    }
    
    repairEdges_(V_);
    
    printProgressBar_(1.0*i / (NUM_ITERATIONS_ - 1));
  }
  
  std::cout<<std::endl;
}

void HornSchunck::printInfoText() const
{
  cout<<"Horn&Schunck optical flow algorithm"<<endl;
  cout<<"==================================="<<endl;
  
  cout<<"Input image width: "<<I_[0].width()<<endl;
  cout<<"Input image height: "<<I_[0].height()<<endl;
  
  cout<<"Number of iterations: "<<NUM_ITERATIONS_<<endl;
  cout<<"Alpha: "<<ALPHA_<<endl;
  cout<<"Relaxation coefficient: "<<RELAX_COEFF_<<endl;
  cout<<"Boundary conditions: ";
  if(BOUNDARY_CONDITIONS_ == NEUMANN)
    cout<<"Neumann"<<endl;
  else
    cout<<"Dirichlet"<<endl;
}

void HornSchunck::computeGradients_(const CImg< unsigned char > &I1,
                                    const CImg< unsigned char > &I2)
{
  int x, y;
  double I00, I10, I20, I30; 
  double I01, I11, I21, I31;
  
  Gx_ = CImg< double >(width_, height_);
  Gx_.fill(0);
  Gy_ = CImg< double >(width_, height_);
  Gy_.fill(0);
  Gt_ = CImg< double >(width_, height_);
  Gt_.fill(0);
  
  for(y = 0; y < height_-1; y++)
  {
    for(x = 0; x < width_-1; x++)
    {
      I00 = I1(x, y) * INTENSITY_SCALE_;
      I10 = I1(x+1, y) * INTENSITY_SCALE_;
      I20 = I1(x, y+1) * INTENSITY_SCALE_;
      I30 = I1(x+1, y+1) * INTENSITY_SCALE_;
      
      I01 = I2(x, y) * INTENSITY_SCALE_;
      I11 = I2(x+1, y) * INTENSITY_SCALE_;
      I21 = I2(x, y+1) * INTENSITY_SCALE_;
      I31 = I2(x+1, y+1) * INTENSITY_SCALE_;
      
      Gx_(x, y) = (I10 - I00 + I30 - I20 + I11 - I01 + I31 - I21) / 4.0;
      Gy_(x, y) = (I20 - I00 + I30 - I10 + I21 - I01 + I31 - I11) / 4.0;
      Gt_(x, y) = (I01 - I00 + I11 - I10 + I21 - I20 + I31 - I30) / 4.0;
    }
  }
  
  // Copy edge values from neighbours.
  for(y = 0; y < height_; y++)
  {
    Gx_(x, height_-1) = Gx_(x, height_-2);
    Gy_(x, height_-1) = Gy_(x, height_-2);
    Gt_(x, height_-1) = Gt_(x, height_-2);
  }
  
  for(y = 0; y < width_; y++)
  {
    Gx_(width_-1, y) = Gx_(width_-2, y);
    Gy_(width_-1, y) = Gy_(width_-2, y);
    Gt_(width_-1, y) = Gt_(width_-2, y);
  }
}

void HornSchunck::printProgressBar_(double pct)
{
  cout<< "[";
  int pos = 70 * pct;
  for (int i = 0; i < 70; i++)
  {
    if (i < pos)
      std::cout << "=";
    else if (i == pos)
      std::cout << ">";
    else
      std::cout << " ";
  }
  std::cout << "] " << int(pct * 100.0) << " %\r";
  std::cout.flush();
}

void HornSchunck::repairEdges_(CImg< double > &V)
{
  int i, x, y;
  
  for(i = 0; i < 2; i++)
  {
    // top and bottom edges
    for(x = 1; x < width_ - 1; x++)
    {
      V(x, 0, i) = V(x, 1, i);
      V(x, height_ - 1, i) = V(x, height_ - 2, i);
    }
    
    // left and right edges
    for(y = 1; y < height_ - 1; y++)
    {
      V(0, y, i) = V(1, y, i);
      V(width_ - 1, y, i) = V(width_ - 2, y, i);
    }
    
    // corners
    V(0, 0, i) = V(1, 1, i);
    V(width_ - 1, 0, i) = V(width_ - 2, 1, i);
    V(0, height_ - 1, i) = V(1, height_ - 2, i);
    V(width_ - 1, height_ - 1, i) = V(width_ - 2, height_ - 2, i);
  }
}
