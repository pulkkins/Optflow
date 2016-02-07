
#ifndef PROESMANS_H

#include "DualDenseMotionExtractor.h"

#include "CImg_config.h"
#include <CImg.h>
#include <string>

using namespace cimg_library;
using namespace std;

/// Implements the Proesmans algorithm for motion extraction.
/**
 * This implementation is based on the following article:
 * 
 * M. Proesmans, L. Van Gool, E. Pauwels, and A. Oosterlinck, 
 * Determination of optical ﬂow and its discontinuities using 
 * non-linear diffusion, in 3rd European Conference on Computer 
 * Vision, ECCV’94, 1994, Vol. 2, pp. 295–304.
 *
 * and the C-implementation found at http://of-eval.sourceforge.net
 */
class Proesmans : public DualDenseMotionExtractor
{
public:
  enum BoundaryConditions { DIRICHLET, NEUMANN };
  
  /// Default constructor.
  /**
   * Constructs a Proesmans motion extractor with the default parameters.
   * - number of iterations = 200
   * - lambda = 100
   * - boundary conditions = Neumann
   */
  Proesmans();
  
  /// Parametrized constructor.
  Proesmans(int numIterations_,
            float lambda_,
            BoundaryConditions boundaryConditions_);
  
  void compute(const CImg< unsigned char > &I1,
               const CImg< unsigned char > &I2,
               CImg< double > &VF,
               CImg< double > &VB);
  
  BoundaryConditions getBoundaryConditions() const;
  
  double getLambda() const;
  
  string getName() const;
  
  int getNumIterations() const;
  
  int getNumResultQualityChannels() const;
  
  bool isDual() const;
  
  void printInfoText() const;
private:
  const BoundaryConditions BOUNDARY_CONDITIONS_;
  const bool COMPUTE_RESIDUALS_;
  const double INTENSITY_SCALE_;
  const double LAMBDA_;
  const int NUM_ITERATIONS_;
  
  CImg< unsigned char > I_[2];
  CImg< double > gamma_[2];
  CImg< double > G_[2];
  CImg< double > V_[2];
  
  int width_, height_;
  
  double computeAvg_(int x,
                     int y,
                     const CImg< double > &gi,
                     const CImg< double > &Vi,
                     double *v);
  
  void computeGradients_(const CImg< unsigned char > &I, 
                         CImg< double > &G);
  
  void computeConsistencyMaps_();
  
  void IterationStep_(double gx,
                      double gy,
                      double It,
                      double *avg,
                      double *result);
  
  void printProgressBar_(double pct);
  
  void repairEdges_(CImg< double > &V);
};

#define PROESMANS_H

#endif
