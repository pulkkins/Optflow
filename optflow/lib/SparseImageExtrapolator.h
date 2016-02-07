
#ifdef WITH_CGAL
#ifndef SPARSEIMAGEEXTRAPOLATOR_H

#include "SparseVectorField.h"

#include <list>
#include <vector>

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;
using namespace std;

// TODO: consider making this an abstract base class

/// Implements a triangulation-based image extrapolator.
/**
 * This class implements a forward-mapping image 
 * extrapolator that uses sparse vector fields. The 
 * algorithm uses a triangulation of the vector field. 
 * The original image is used as a texture 
 * for triangles that are warped by using 
 * the motion vectors at their vertices.
 */
class SparseImageExtrapolator
{
public:
  /// Extrapolates the source image by using a dense vector field.
  /**
   * This uses a uniformly spaced sparse vector 
   * field generated from a dense motion vector field.
   * @param I0 the image to extrapolate
   * @param V the vector field to use
   * @param multiplier multiplier for motion vector magnitudes
   * @param spacing number of pixels between motion vectors
   * @param Ie the extrapolated image
   */
  void extrapolate(const CImg< unsigned char > &I0,
                   const CImg< double > &V,
                   double multiplier,
                   double spacing,
                   CImg< unsigned char > &Ie) const;
  
  /// Extrapolates the source image by using a sparse vector field.
  /**
   * @param I0 the image to extrapolate
   * @param V the vector field to use
   * @param multiplier multiplier for motion vector magnitudes
   * @param Ie the extrapolated image
   */
  void extrapolate(const CImg< unsigned char > &I0,
                   const SparseVectorField &V,
                   double multiplier,
                   CImg< unsigned char > &Ie) const;
private:
  void extrapolate_(const CImg< unsigned char > &I0,
                    const vector< Point > &featurePoints1,
                    const vector< Point > &featurePoints2,
                    const list< int > &triIndices,
                    double multiplier,
                    CImg< unsigned char > &Ie) const;
  
  void constructUniformMesh(int w, int h, int n,
                            vector< Point > &points,
                            list< int > &indices) const;
};

#define SPARSEIMAGEEXTRAPOLATOR_H

#endif
#endif
