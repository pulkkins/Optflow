
#ifndef MOTIONEXTRACTORDRIVER_H

#include "DenseMotionExtractor.h"
#include "SparseMotionExtractor.h"

#include <string>

class SparseVectorField;

using namespace std;

/// Implements methods for running motion extractors and saving the results to disk.
/**
 * The methods defined here invoke motion extraction algorithms to extract 
 * motion between two source images. These methods also slightly blur the source 
 * images in order to achieve better results (gradient computation is sensitive 
 * to noise). The blurred images and the resulting motion image and motion 
 * vector quality images are saved with the following names:
 * 
 * [basename1]-smoothed.[extension]
 * [basename2]-smoothed.[extension]
 * [prefix]-motion.png
 * [prefix]-quality[n].png
 * 
 * where "basename1" and "basename2" are the names of the source images 
 * excluding their extensions and "prefix" is given by user.
 */
namespace MotionExtractorDriver
{
  /// Runs a dense motion extractor.
  /**
   * @param e motion extraction algorithm
   * @param srcFileName1 the file to read the first source image from
   * @param srcFileName2 the file to read the second source image from
   * @param outFileNamePrefix the prefix of the resulting images
   */
  void runDenseMotionExtractor(DenseMotionExtractor &e,
                               const string &src1,
                               const string &src2,
                               const string &outFilePrefix);
  
#ifdef WITH_CGAL
  /// Runs a sparse motion extractor.
  /**
   * @param e motion extraction algorithm
   * @param srcFileName1 the file to read the first source image from
   * @param srcFileName2 the file to read the second source image from
   * @param outFileNamePrefix the prefix of the resulting images
   */
  void runSparseMotionExtractor(SparseMotionExtractor &e,
                                const string &src1,
                                const string &src2,
                                const string &outFilePrefix);
#endif
};

#define MOTIONEXTRACTORDRIVER_H

#endif
