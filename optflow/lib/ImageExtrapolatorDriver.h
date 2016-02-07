
#ifndef EXTRAPOLATORDRIVER_H

#include <string>

namespace cimg_library { template < class T > class CImg; }
class DenseImageExtrapolator;
class DenseVectorField;
class SparseImageExtrapolator;
class SparseVectorField;

using namespace cimg_library;
using namespace std;

/// Implements driver methods for running image extrapolators and writing results to disk.
/**
 * The names of the warped images are of the form [prefix]-[n].png, where 
 * n is the index of the image in the warped sequence.
 */
class ImageExtrapolatorDriver
{
public:
  /// Runs a dense image extrapolator.
  /**
   * @param e image extrapolation algorithm
   * @param I0 the image to extrapolate
   * @param V the motion vector field to use
   * @param numSteps the number of images to generate by successively applying V
   * @param resultFileNamePrefix the prefix for the resulting image files
   */
  static void runDenseImageExtrapolator(const DenseImageExtrapolator &e,
                                        const CImg< unsigned char > &I0,
                                        const CImg< double > &V,
                                        int numSteps,
                                        const string &resultFileNamePrefix);
  
#ifdef WITH_CGAL
  /// Runs a sparse image extrapolator.
  /**
   * @param e image extrapolation algorithm
   * @param I0 the image to extrapolate
   * @param V the motion vector field to use
   * @param numSteps the number of images to generate by successively applying V
   * @param resultFileNamePrefix prefix for the resulting image files
   */
  static void runSparseImageExtrapolator(const SparseImageExtrapolator &e,
                                         const CImg< unsigned char > &I0,
                                         const SparseVectorField &V,
                                         int numSteps,
                                         const string &resultFileNamePrefix);
  
  /// Runs a sparse image extrapolator.
  /**
   * This method uses a dense motion field from which 
   * the vectors are extracted by using an uniform grid.
   * @param e image extrapolation algorithm
   * @param I0 the image to extrapolate
   * @param V the motion vector field to use
   * @param numSteps the number of images to generate by successively applying V
   * @param spacing spacing between motion vectors
   * @param resultFileNamePrefix prefix for the resulting image files
   */
  static void runSparseImageExtrapolator(const SparseImageExtrapolator &e,
                                         const CImg< unsigned char > &I0,
                                         const CImg< double > &V,
                                         int numSteps,
                                         double spacing,
                                         const string &resultFileNamePrefix);
#endif
};

#define EXTRAPOLATORDRIVER_H

#endif
