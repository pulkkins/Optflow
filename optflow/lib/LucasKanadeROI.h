
#ifndef LUCASKANADEROI_H

#include "ROI.h"

#include "CImg_config.h"
#include <CImg.h>

using namespace cimg_library;
using namespace std;

/// Implements a ROI for the Lucas and Kanade algorithm. 
/**
 * For a given region of interest, the Lucas and Kanade 
 * algorithm maintains the derivative matrix
 *
 *      | I_x^2     I_x*I_y |
 *  G = |                   |
 *      | I_x*I_y   I_y^2   |,
 *
 * where I_x and I_y denote partial derivatives with 
 * respect to x and y.
 */
class LucasKanadeROI : public ROI
{
public:
  LucasKanadeROI() { }

  LucasKanadeROI(int x, int y, int w, int h,
                 const CImg< unsigned char > &I_,
                 const CImg< double > &G_);

  LucasKanadeROI(int x, int y, int w, int h,
                 const CImg< unsigned char > &I_,
                 const CImg< double > &G_,
                 const CImg< double > *W);

  void addNewTerm(int x, int y);

  double computeDeterminant() const;

  double getGWGElement(int i, int j) const;

  //double getRHSTerm(int i);

  void initialize();

  void subtractOldTerm(int x, int y);
private:
  double GWG[2][2];
  CImg< double > G;
  CImg< unsigned char > I;
};

#define LUCASKANADEROI_H

#endif
