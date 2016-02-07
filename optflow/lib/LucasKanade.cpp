
#include "LucasKanade.h"
#include "ROI.h"

#include <iostream>
#include <math.h>

#include "CImg_config.h"
#include <CImg.h>

LucasKanade::LucasKanade() : COMPUTE_RESIDUALS_(true),
                             INTENSITY_SCALE_(1.0 / 255.0),
                             WINDOW_RADIUS_(16),
                             NUM_ITERATIONS_(5),
                             TAU_(0.0025),
                             SIGMAP_(0.0),
                             WINDOW_SIZE_(2 * WINDOW_RADIUS_ + 1)
{
  W_ = new CImg< double >(WINDOW_SIZE_, WINDOW_SIZE_);
  double c[1] = { 1.0 };
  W_->draw_gaussian(WINDOW_SIZE_ / 2.0f, WINDOW_SIZE_ / 2.0f, WINDOW_RADIUS_ / 3.0f, &c[0]);
}

LucasKanade::LucasKanade(int windowRadius,
                         int numIterations,
                         float tau,
                         float sigmap,
                         bool useWeightingKernel) : 
  COMPUTE_RESIDUALS_(true),
  INTENSITY_SCALE_(1.0 / 255.0),
  WINDOW_RADIUS_(windowRadius),
  NUM_ITERATIONS_(numIterations),
  TAU_(tau),
  SIGMAP_(sigmap),
  WINDOW_SIZE_(2 * WINDOW_RADIUS_ + 1)
{
  if(useWeightingKernel)
  {
    W_ = new CImg< double >(WINDOW_SIZE_, WINDOW_SIZE_);
    double c[1] = { 1.0 };
    W_->draw_gaussian(WINDOW_SIZE_ / 2.0f, WINDOW_SIZE_ / 2.0f, WINDOW_RADIUS_ / 3.0f, &c[0]);
  }
  else
    W_ = NULL;
}

LucasKanade::~LucasKanade()
{
  if (W_ != NULL)
    delete W_;
}

void LucasKanade::compute(const CImg< unsigned char > &I1,
                          const CImg< unsigned char > &I2,
                          CImg< double > &V)
{
  int baseIndex = 0, index;
  int i;
  int x, y;
  LSQInput lsqInput;
  LSQResults lsqResults;
  
  width_  = I1.width();
  height_ = I1.height();
  
  I1_.assign(I1, true);
  I2_.assign(I2, true);
  
  computeGradients_(I1_, G1_);
  
  if(COMPUTE_RESIDUALS_ == true)
  {
    residualSum_ = 0.0;
    maxResidual_ = 0.0;
    numResidualSumTerms_ = 0;
  }
  
  roi_ = LucasKanadeROI(-WINDOW_RADIUS_, -WINDOW_RADIUS_,
                        WINDOW_SIZE_, WINDOW_SIZE_,
                        I1_, G1_, W_);
  roi_.initialize();
  
  for(y = 0; y < height_; y++)
  {
    index = baseIndex;

    for(x = 0; x < width_; x++)
    {
      lsqInput.x = x;
      lsqInput.y = y;
      lsqInput.ivx = V(x, y, 0, 0);
      lsqInput.ivy = V(x, y, 0, 1);
      
      computeLSQVelocity_(lsqInput, lsqResults);
      
      V(x, y, 0, 0) = lsqResults.vx;
      V(x, y, 0, 1) = lsqResults.vy;
      for(i = 0; i < getNumResultQualityChannels(); i++)
	V(x, y, 0, 2 + i) = lsqResults.quality[i];
      
      index++;
      roi_.translate(1, 0);
    }
    
    baseIndex += width_;
    roi_.translate(-width_, 1);
  }
}

string LucasKanade::getName() const
{
  return "Lucas-Kanade";
}

int LucasKanade::getNumIterations() const
{
  return NUM_ITERATIONS_;
}

int LucasKanade::getNumResultQualityChannels() const
{
  return 2;
}

double LucasKanade::getSigmap() const
{
  return SIGMAP_;
}

double LucasKanade::getTau() const
{
  return TAU_;
}

int LucasKanade::getWindowSize() const
{
  return WINDOW_SIZE_;
}

bool LucasKanade::isDual() const
{
  return false;
}

void LucasKanade::printInfoText() const
{
  cout<<"Lucas & Kanade optical flow algorithm"<<endl;
  cout<<"====================================="<<endl;
  
  cout<<"Input image width: "<<I1_.width()<<endl;
  cout<<"Input image height: "<<I1_.height()<<endl;
  
  cout<<"Window size: "<<WINDOW_SIZE_<<endl;
  cout<<"Number of Gauss-Newton iterations: "<<NUM_ITERATIONS_<<endl;
  cout<<"Tau (eigenvalue threshold): "<<TAU_<<endl;
  cout<<"Sigmap (regularization parameter): "<<SIGMAP_<<endl;
}

// Computes eigenvalues of a 2x2 matrix
// |a   b|
// |b   c|.
void LucasKanade::computeEigenValues_(double a, double b, double c,
                                      double &lambda1, double &lambda2)
{
  lambda1 = 0.5 * (a + c + sqrt(4.0*b*b + (a - c)*(a - c)));
  lambda2 = 0.5 * (a + c - sqrt(4.0*b*b + (a - c)*(a - c)));
}

void LucasKanade::computeGradients_(const CImg< unsigned char > &I,
                                    CImg< double > &G)
{
  G = CImg< double >(width_, height_, 1, 2);
  
  CImg< double > Kx = CImg< double >(5, 1);
  CImg< double > Ky = CImg< double >(1, 5);
  
  Kx(0, 0) = -1.0 / 12.0 * INTENSITY_SCALE_;
  Kx(1, 0) = 8.0 / 12.0 * INTENSITY_SCALE_;
  Kx(2, 0) = 0.0;
  Kx(3, 0) = -8.0 / 12.0 * INTENSITY_SCALE_;
  Kx(4, 0) = 1.0 / 12.0 * INTENSITY_SCALE_;
  
  Ky(0, 0) = -1.0 / 12.0 * INTENSITY_SCALE_;
  Ky(0, 1) = 8.0 / 12.0 * INTENSITY_SCALE_;
  Ky(0, 2) = 0.0;
  Ky(0, 3) = -8.0 / 12.0 * INTENSITY_SCALE_;
  Ky(0, 4) = 1.0 / 12.0 * INTENSITY_SCALE_;
  
  G.get_shared_channel(0) = I.get_convolve(Kx, 0);
  G.get_shared_channel(1) = I.get_convolve(Ky, 0);
}

void LucasKanade::computeLSQVelocity_(const LSQInput &input,
                                      LSQResults &results)
{
  bool accepted = false;
  double D;
  double deltavx, deltavy;
  int i;
  double I1s, I2s;
  double IDiff;
  double lambda1 = 0.0, lambda2 = 0.0;
  double gxs, gys;
  double prevrSum = 0.0, rSum = 0.0;
  double r = 0.0;
  double smallerLambda;
  double sumdx;
  double sumdy;
  double sumx2 = 0.0;
  double sumy2 = 0.0;
  double sumxy = 0.0;
  double xd, yd;
  int xw, yw;
  double w;
  int x1Abs, y1Abs;
  double x2Abs, y2Abs;
  
  results.vx = input.ivx;
  results.vy = input.ivy;
  
  sumx2 = roi_.getGWGElement(0, 0);
  sumxy = roi_.getGWGElement(0, 1);
  sumy2 = roi_.getGWGElement(1, 1);
  D = roi_.computeDeterminant();
  
  computeEigenValues_(sumx2, sumxy, sumy2, lambda1, lambda2);
  smallerLambda = min(lambda1, lambda2);
  
  if(smallerLambda > TAU_)
  {
    accepted = true;
    
    for(i = 0; i < NUM_ITERATIONS_; i++)
    {
      sumdx = 0.0;
      sumdy = 0.0;
      
      if(i > 0)
        prevrSum = rSum;
      rSum = 0.0;
      
      xd = input.x + results.vx;
      yd = input.y + results.vy;
      
      for(yw = -WINDOW_RADIUS_; yw <= WINDOW_RADIUS_; yw++)
      {
        for(xw = -WINDOW_RADIUS_; xw <= WINDOW_RADIUS_; xw++)
        {
          x1Abs = input.x + xw;
          y1Abs = input.y + yw;
          x2Abs = xd + xw;
          y2Abs = yd + yw;
          
          gxs = G1_.linear_atXY(x1Abs, y1Abs, 0, 0);
          gys = G1_.linear_atXY(x1Abs, y1Abs, 0, 1);
          
          I1s = I1_.linear_atXY(x1Abs, y1Abs) * INTENSITY_SCALE_;
          I2s = I2_.linear_atXY(x2Abs, y2Abs) * INTENSITY_SCALE_;
          
          if(W_ != NULL)
            w = (*W_)(xw + WINDOW_RADIUS_, yw + WINDOW_RADIUS_);
          else
            w = 1.0;
          
          IDiff = I2s - I1s;
          
          sumdx += w * IDiff * gxs;
          sumdy += w * IDiff * gys;
          
          rSum += w * IDiff * IDiff;
        }
      }
      
      /*sumdx -= roi.getRHSTerm(0) * INTENSITY_SCALE;
      sumdy -= roi.getRHSTerm(1) * INTENSITY_SCALE;*/
      
      deltavx = (sumy2*sumdx - sumxy*sumdy) / D;
      deltavy = (-sumxy*sumdx + sumx2*sumdy) / D;
      
      // Terminate the iteration if it doesn't converge. 
      if(rSum > prevrSum && i > 0)
      {
        rSum = prevrSum;
        break;
      }
      
      results.vx -= deltavx;
      results.vy -= deltavy;
    }
  }
  
  if(COMPUTE_RESIDUALS_ == true && accepted == true)
  {
    r = rSum / (WINDOW_SIZE_ * WINDOW_SIZE_);
    residualSum_ += r;
    
    if(r > maxResidual_)
      maxResidual_ = r;
    
    numResidualSumTerms_++;
  }
  
  results.quality[0] = max(0.0, min(1.0, smallerLambda));
  results.quality[1] = min(255.0, 255.0 / (1000.0*r + 1.0))  / 255.0;
}
