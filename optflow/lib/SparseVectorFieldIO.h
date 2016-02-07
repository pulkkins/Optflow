
#ifdef WITH_CGAL
#ifndef SPARSEVECTORFIELDIO_H

#include <string>

using namespace std;

class SparseVectorField;

/// Implements methods for reading and writing sparse motion vector fields.
/**
 * This class implements methods for reading and writing sparse 
 * motion vector fields in PSVM format (an extension of the PGM format).
 *
 * A PSVM header is of the form:
 * PSV
 * [numvectors]
 *
 * The data consists of sequentially 
 * ordered quartets. The first two elements of nth 
 * quartet specify the starting point of the nth 
 * vector and the last two elements specify the end 
 * point of the nth vector.
 */
class SparseVectorFieldIO
{
public:
  /// Reads a vector field from a file in PSVM format.
  static void readVectorField(const string &inFileName,
                              SparseVectorField &V);
  
  /// Writes a vector field to a file in PSVM format.
  static void writeVectorField(const SparseVectorField &V,
                               const string &outFileName);
};

#define SPARSEVECTORFIELDIO_H

#endif
#endif
