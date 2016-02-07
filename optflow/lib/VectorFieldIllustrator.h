
#ifndef VECTORFIELDILLUSTRATOR_H

#include "SparseVectorField.h"

#include <list>
#include <vector>

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;
using namespace std;

/// Implements methods for rendering colored arrow images from vector fields.
/**
 * The methods in this class render arrow images from vector fields. 
 * Arrow colors are taken from motion vector qualities 
 * (the first quality number, if the vector field contains several 
 * quality indicators).
 */
class VectorFieldIllustrator
{
public:
  /// Renders an arrow image from a dense vector field.
  /**
   * @param[in] V the vector field to draw
   * @param[out] I the image to draw into
   * @param[in] spacing spacing between arrows (in pixels)
   */
  static void renderDenseVectorField(const CImg< double > &V,
                                     CImg< unsigned char > &I,
                                     int spacing = 15);
  
#ifdef WITH_CGAL
  /// Renders an arrow image from a sparse vector field.
  /**
   * @param[in] V the vector field to draw
   * @param[out] I the image to draw into
   */
  static void renderSparseVectorField(const SparseVectorField &V,
                                      CImg< unsigned char > &I);
#endif
};

#define VECTORFIELDILLUSTRATOR_H

#endif
