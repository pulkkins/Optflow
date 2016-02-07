
#ifndef LUCASKANADE_H

#include "LucasKanadeROI.h"
#include "DenseMotionExtractor.h"

#include <string>

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;

/// Implements single-resolution Lucas and Kanade motion extractor.
/**
 * This implementation is based on the following article:
 *
 * B.D. Lucas and T. Kanade, An iterative image registration 
 * technique with an application to stereo vision, in Proc. 
 * Seventh International Joint Conference on Artificial 
 * Intelligence, Vancouver, 1981, pp. 674-679
 *
 * and the C-implementation found at ftp://ftp.csd.uwo.ca/pub/vision/LUCAS
 */
class LucasKanade : public DenseMotionExtractor
{
 public:
  /// Default constructor. 
  /**
   * Constructs a Lucas and Kanade motion extractor with the default parameters.
   * - window size = 33
   * - number of iterations = 5
   * - tau = 0.0025
   * - sigmap = 0
   */
  LucasKanade();
  
  /// Parametrized constructor.
  LucasKanade(int windowRadius,
              int numIterations,
              float tau,
              float sigmap,
              bool useWeightingKernel);
  
  ~LucasKanade();
  
  void compute(const CImg< unsigned char > &I1,
               const CImg< unsigned char > &I2,
               CImg< double > &V);
  
  string getName() const;
  
  int getNumIterations() const;
  
  int getNumResultQualityChannels() const;
  
  double getSigmap() const;
  
  double getTau() const;
  
  int getWindowSize() const;
  
  bool isDual() const;
  
  void printInfoText() const;
private:
  struct LSQInput
  {
    int x, y;
    double ivx, ivy;
  };
  
  struct LSQResults
  {
    double vx, vy;
    double quality[2];
  };
  
  const bool COMPUTE_RESIDUALS_;
  const double INTENSITY_SCALE_;
  
  const int WINDOW_RADIUS_;
  const int NUM_ITERATIONS_;
  const double TAU_;
  const double SIGMAP_;
  const int WINDOW_SIZE_;
  
  double residualSum_;
  double maxResidual_;
  int numResidualSumTerms_;
  LucasKanadeROI roi_;
  CImg< double > *W_;
  int width_, height_;
  CImg< unsigned char > I1_, I2_;
  CImg< double > G1_;
  
  void computeEigenValues_(double a, double b, double c,
                           double &lambda1, double &lambda2);
  
  void computeGradients_(const CImg< unsigned char > &I,
                         CImg< double > &G);
  
  void computeLSQVelocity_(const LSQInput &input, LSQResults &results);
};

#define LUCASKANADE_H

#endif
