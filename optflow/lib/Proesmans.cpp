
#include "Proesmans.h"

#include <iostream>
#include <math.h>

Proesmans::Proesmans() : BOUNDARY_CONDITIONS_(NEUMANN),
                         COMPUTE_RESIDUALS_(false),
                         INTENSITY_SCALE_(1.0 / 255.0),
                         LAMBDA_(100.0),
                         NUM_ITERATIONS_(200)
{ }

Proesmans::Proesmans(int numIterations_,
                     float lambda_,
                     BoundaryConditions boundaryConditions_) : 
  BOUNDARY_CONDITIONS_(boundaryConditions_),
  COMPUTE_RESIDUALS_(false),
  INTENSITY_SCALE_(1.0 / 255.0),
  LAMBDA_(lambda_),
  NUM_ITERATIONS_(numIterations_)
{ }

void Proesmans::compute(const CImg< unsigned char > &I1,
                        const CImg< unsigned char > &I2,
                        CImg< double > &VF,
                        CImg< double > &VB)
{
  int i, j;
  int x, y;
  double vAvg[2];
  double xd, yd;
  double It;
  double vNext[2];
  
  width_ = I1.width();
  height_ = I1.height();
  
  I_[0].assign(I1, true);
  I_[1].assign(I2, true);
  
  V_[0].assign(VF, true);
  V_[1].assign(VB, true);
  
  computeGradients_(I_[0], G_[0]);
  computeGradients_(I_[1], G_[1]);
  
  // TODO: Should gamma be a component of V_ (and thus interpolated between pyramid levels)?
  gamma_[0] = CImg< double >(width_, height_);
  gamma_[1] = CImg< double >(width_, height_);
  
  for(i = 0; i < NUM_ITERATIONS_; i++)
  {
    if(i < NUM_ITERATIONS_)
      computeConsistencyMaps_();
    
    for(y = 1; y < height_ - 1; y++)
    {
      for(x = 1; x < width_ - 1; x++)
      {
        for(j = 0; j < 2 ; j++)
        {
          computeAvg_(x, y, gamma_[j], V_[j], &vAvg[0]);
          
          xd = x + vAvg[0];
          yd = y + vAvg[1];
          
          //iteration step
          if(xd >= 0 && xd <= width_ - 1 && yd >= 0 && yd <= height_ - 1)
          {
            It = (I_[1 - j].linear_atXY(xd, yd) - I_[j].atXY(x, y)) * INTENSITY_SCALE_;
            IterationStep_(G_[j](x, y, 0, 0), G_[j](x, y, 0, 1), It, vAvg, &vNext[0]);
          }
          else
          {
            // use consistency-weighted average as the next value 
            // if (xd,yd) is outside the image
            vNext[0] = vAvg[0];
            vNext[1] = vAvg[1];
          }

          if(i < NUM_ITERATIONS_)
          {
            V_[j](x, y, 0, 0) = vNext[0];
            V_[j](x, y, 0, 1) = vNext[1];
          }
        }
        
        // store quality information (gamma)
        if(i < NUM_ITERATIONS_)
        {
          VF(x, y, 2) = gamma_[0](x, y);
          VB(x, y, 2) = gamma_[1](x, y);
        }
      }
    }
    
    if(i < NUM_ITERATIONS_ && BOUNDARY_CONDITIONS_ == NEUMANN)
    {
      repairEdges_(V_[0]);
      repairEdges_(V_[1]);
    }
    
    printProgressBar_(1.0*i / (NUM_ITERATIONS_ - 1));
  }
  
  std::cout<<std::endl;
}

Proesmans::BoundaryConditions Proesmans::getBoundaryConditions() const
{
  return BOUNDARY_CONDITIONS_;
}

double Proesmans::getLambda() const
{
  return LAMBDA_;
}

string Proesmans::getName() const
{
  return "Proesmans";
}

int Proesmans::getNumIterations() const
{
  return NUM_ITERATIONS_;
}

int Proesmans::getNumResultQualityChannels() const
{
  return 1;
}

bool Proesmans::isDual() const
{
  return true;
}

inline void Proesmans::IterationStep_(double gx,
                                      double gy,
                                      double It,
                                      double *avg,
                                      double *result)
{
  double m = LAMBDA_ * It / (1.0 + LAMBDA_ * (gx * gx + gy * gy));

  result[0] = avg[0] - gx * m;
  result[1] = avg[1] - gy * m;
}

void Proesmans::printInfoText() const
{
  cout<<"Proesmans' optical flow algorithm"<<endl;
  cout<<"================================="<<endl;
  
  cout<<"Input image width: "<<I_[0].width()<<endl;
  cout<<"Input image height: "<<I_[0].height()<<endl;
  
  cout<<"Number of iterations: "<<NUM_ITERATIONS_<<endl;
  cout<<"Lambda: "<<LAMBDA_<<endl;
  cout<<"Boundary conditions: ";
  if(BOUNDARY_CONDITIONS_ == NEUMANN)
    cout<<"Neumann"<<endl;
  else
    cout<<"Dirichlet"<<endl;
}

inline double Proesmans::computeAvg_(int x, int y,
                                     const CImg< double > &gi,
                                     const CImg< double > &Vi,
                                     double *v)
{
  double sumWeights = (gi(x, y-1)   + gi(x-1, y) +
                       gi(x+1, y)   + gi(x, y+1)) / 6.0 +
                      (gi(x-1, y-1) + gi(x+1, y-1) +
                       gi(x-1, y+1) + gi(x+1, y+1)) / 12.0;
  
  for(int j = 0; j < 2; j++)
  {
    if(sumWeights > 1e-8)
    {
      v[j] = (gi(x, y-1)   * Vi(x, y-1, j)   + gi(x-1, y)   * Vi(x-1, y, j) + 
              gi(x+1, y)   * Vi(x+1, y, j)   + gi(x, y+1)   * Vi(x, y+1, j))   / 6.0 + 
             (gi(x-1, y-1) * Vi(x-1, y-1, j) + gi(x+1, y-1) * Vi(x+1, y-1, j) + 
              gi(x-1, y+1) * Vi(x-1, y+1, j) + gi(x+1, y+1) * Vi(x+1, y+1, j)) / 12.0;
      
      v[j] /= sumWeights;
    }
    else
      // use the old value if the weight sum is too small to give accurate results
      v[j] = Vi(x, y, j);
  }
  
  // Reject too small values for numerical stability.
  if(sumWeights > 1e-8)
    return sumWeights;
  else
    return 1.0;
}

void Proesmans::computeConsistencyMaps_()
{
  int x, y;
  int i;
  //int xd, yd;
  double xd, yd;
  double ub, vb;
  double uDiff, vDiff;
  double c;
  double CSum;
  int CCount;
  double K;
  double g;
  
  for(i = 0; i < 2; i++)
  {
    CSum = 0.0;
    CCount = 0;
    
    for(y = 0; y < height_; y++)
    {
      for(x = 0; x < width_; x++)
      {
        /*xd = (int)(x + V_[i](x, y, 0, 0));
        yd = (int)(y + V_[i](x, y, 0, 1));*/
        xd = x + V_[i](x, y, 0, 0);
        yd = y + V_[i](x, y, 0, 1);
        
        if(xd >= 0 && yd >= 0 && xd <= width_ - 1 && yd <= height_ - 1)
        {
          /*ub = V_[1 - i](xd, yd, 0, 0);
          vb = V_[1 - i](xd, yd, 0, 1);*/
          ub = V_[1 - i].linear_atXY(xd, yd, 0, 0);
          vb = V_[1 - i].linear_atXY(xd, yd, 0, 1);
          
          uDiff = V_[i](x, y, 0, 0) + ub;
          vDiff = V_[i](x, y, 0, 1) + vb;
          
          c = sqrt(uDiff * uDiff + vDiff * vDiff);
          
          gamma_[i](x, y) = c;
          CSum += c;
          CCount++;
        }
        else
          gamma_[i](x, y) = -1.0;
      }
    }
    
    if(CCount > 0)
    {
      K = 0.9 * CSum / CCount;
      
      if(K > 0.0)
      {
        for(y = 0; y < height_; y++)
        {
          for(x = 0; x < width_; x++)
          {
            if(gamma_[i](x, y) >= 0.0)
            {
              g = gamma_[i](x, y);
              gamma_[i](x, y) = 1.0 / (1.0 + (g / K) * (g / K));
            }
            else
              gamma_[i](x, y) = 0.0;
          }
        }
      }
    }
  }
}

void Proesmans::computeGradients_(const CImg< unsigned char > &I,
                                  CImg< double > &G)
{
  // This uses 3x3 Sobel kernels for computing partial derivatives.
  G = CImg< double >(width_, height_, 1, 2);
  CImg< double > Kx = CImg< double >(3, 3);
  CImg< double > Ky = CImg< double >(3, 3);
  
  Kx(0, 0) = 1.0 / 8.0 * INTENSITY_SCALE_;
  Kx(1, 0) = 0.0;
  Kx(2, 0) = -1.0 / 8.0 * INTENSITY_SCALE_;
  Kx(0, 1) = 2.0 / 8.0 * INTENSITY_SCALE_;
  Kx(1, 1) = 0.0;
  Kx(2, 1) = -2.0 / 8.0 * INTENSITY_SCALE_;
  Kx(0, 2) = 1.0 / 8.0 * INTENSITY_SCALE_;
  Kx(1, 2) = 0.0;
  Kx(2, 2) = -1.0 / 8.0 * INTENSITY_SCALE_;
  
  Ky(0, 0) = 1.0 / 8.0 * INTENSITY_SCALE_;
  Ky(1, 0) = 2.0 / 8.0 * INTENSITY_SCALE_;
  Ky(2, 0) = 1.0 / 8.0 * INTENSITY_SCALE_;
  Ky(0, 1) = 0.0;
  Ky(1, 1) = 0.0;
  Ky(2, 1) = 0.0;
  Ky(0, 2) = -1.0 / 8.0 * INTENSITY_SCALE_;
  Ky(1, 2) = -2.0 / 8.0 * INTENSITY_SCALE_;
  Ky(2, 2) = -1.0 / 8.0 * INTENSITY_SCALE_;
  
  G.get_shared_channel(0) = I.get_convolve(Kx, 0);
  G.get_shared_channel(1) = I.get_convolve(Ky, 0);
}

void Proesmans::printProgressBar_(double pct)
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

void Proesmans::repairEdges_(CImg< double > &V)
{
  int x, y;
  int i;
  
  for(i = 0; i < 2; i++)
  {
    // top and bottom edges
    for(x = 1; x < width_ - 1; x++)
    {
      V(x, 0, 0, i) = V(x, 1, 0, i);
      V(x, height_ - 1, 0, i) = V(x, height_ - 2, 0, i);
    }
    
    // left and right edges
    for(y = 1; y < height_ - 1; y++)
    {
      V(0, y, 0, i) = V(1, y, 0, i);
      V(width_ - 1, y, 0, i) = V(width_ - 2, y, 0, i);
    }
    
    // corners
    V(0, 0, 0, i) = V(1, 1, 0, i);
    V(width_ - 1, 0, 0, i) = V(width_ - 2, 1, 0, i);
    V(0, height_ - 1, 0, i) = V(1, height_ - 2, 0, i);
    V(width_ - 1, height_ - 1, 0, i) = V(width_ - 2, height_ - 2, 0, i);
  }
}
