
#ifndef VECTORFIELDIO_H

#include <string>

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;
using namespace std;

/// Implements methods for reading and writing dense motion vector fields.
/**
 * This class implements methods for reading and writing dense 
 * motion vector fields in PDVM format (an extension of the PGM format).
 *
 * A PDVM header is of the form:
 * PDV
 * [width] [height]
 * [numqualitychannels]
 *
 * The data consists of 2+q -dimensional sequentially 
 * ordered vectors originating from each pixel in the raster, 
 * q is the number of quality channels.
 */
class DenseVectorFieldIO
{
public:
  /// Reads a vector field from a file in PDVM format.
  static void readVectorField(const string &inFileName,
                              CImg< double > &V);
  
  /// Writes a vector field to a file in PDVM format.
  static void writeVectorField(const CImg< double > &V,
                               const string &outFileName);
};

#define VECTORFIELDIO_H

#endif
