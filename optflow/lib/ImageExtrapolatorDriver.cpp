
#include "DenseImageExtrapolator.h"
#include "SparseImageExtrapolator.h"
#include "ImageExtrapolatorDriver.h"

#include "CImg_config.h"
#include <CImg.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

void ImageExtrapolatorDriver::runDenseImageExtrapolator(const DenseImageExtrapolator &e,
                                                        const CImg< unsigned char > &I0,
                                                        const CImg< double > &V,
                                                        int numSteps,
                                                        const string &resultFileNamePrefix)
{
  CImg< unsigned char > Ie = I0;
  ostringstream ostr;
  
  for(int t = 1; t <= numSteps; t++)
  {
    e.extrapolate(I0, V, t, Ie);
    ostr.clear();
    ostr.str("");
    ostr<<setfill('0')<<setw(2)<<t;
    Ie.save_png((resultFileNamePrefix + "-extrapolated-" + ostr.str() + ".png").c_str());
  }
}

#ifdef WITH_CGAL

void ImageExtrapolatorDriver::runSparseImageExtrapolator(const SparseImageExtrapolator &e,
                                                         const CImg< unsigned char > &I0,
                                                         const SparseVectorField &V,
                                                         int numSteps,
                                                         const string &resultFileNamePrefix)
{
  CImg< unsigned char > Ie = I0;
  ostringstream ostr;
  
  for(int t = 1; t <= numSteps; t++)
  {
    e.extrapolate(I0, V, t, Ie);
    ostr.clear();
    ostr.str("");
    ostr<<setfill('0')<<setw(2)<<t;
    Ie.save_png((resultFileNamePrefix + "-extrapolated-" + ostr.str() + ".png").c_str());
  }
}

void ImageExtrapolatorDriver::runSparseImageExtrapolator(const SparseImageExtrapolator &e,
                                                         const CImg< unsigned char > &I0,
                                                         const CImg< double > &V,
                                                         int numSteps,
                                                         double spacing,
                                                         const string &resultFileNamePrefix)
{
  CImg< unsigned char > Ie = I0;
  ostringstream ostr;
  
  for(int t = 1; t <= numSteps; t++)
  {
    e.extrapolate(I0, V, t, spacing, Ie);
    ostr.clear();
    ostr.str("");
    ostr<<setfill('0')<<setw(2)<<t;
    Ie.save_png((resultFileNamePrefix + "-extrapolated-" + ostr.str() + ".png").c_str());
  }
}

#endif
