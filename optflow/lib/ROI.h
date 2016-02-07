
#ifndef ROI_H

namespace cimg_library { template < class T > class CImg; }

using namespace cimg_library;

/// Defines a ROI (region of interest) with operations for fast updating. 
class ROI
{
public:
  ROI();
  
  /// Constructs a new ROI with a given anchor point, dimensions and optional weighting kernel. 
  /**
   * @param x x-coordinate of the anchor (upper-left) point
   * @param y y-coordinate of the anchor (upper-left) point
   * @param w width
   * @param h height
   * @param W weighting kernel (optional)
   */
  ROI(int x, int y, int w, int h,
      const CImg< double > *W);
  
  virtual ~ROI() { }
  
  /// Adds a new pixel value to this ROI and applies the necessary updates.
  virtual void addNewTerm(int x, int y) = 0;
  
  /// Returns the pixel value computed from the pixel values within ROI.
  virtual double compute() const { return 0.0; }
  
  /// Reinitializes this ROI, i.e. updates all pixel values in the region.
  virtual void initialize() = 0;
  
  /// Removes a value from this ROI and applies the necessary updates.
  virtual void subtractOldTerm(int x, int y) = 0;
  
  /// Translates this ROI located at (x,y) to (x+dx,y+dy).
  void translate(int dx, int dy);
protected:
  int anchorx_, anchory_;
  int width_, height_;
  const CImg< double > *W_;
};

#define ROI_H

#endif
