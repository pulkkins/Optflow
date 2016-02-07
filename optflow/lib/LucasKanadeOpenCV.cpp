
#if defined (WITH_OPENCV) && defined(WITH_CGAL)

#include "LucasKanadeOpenCV.h"

#include "CImg_config.h"
#include <CImg.h>
#include <opencv/cv.h>

LucasKanadeOpenCV::LucasKanadeOpenCV() : 
  EPSILON_(0.001), MAX_NUM_FEATURE_POINTS_(1000),
  MAX_NUM_ITER_(10), MIN_POINT_DISTANCE_(5),
  NUM_PYRAMID_LEVELS_(4), QUALITY_LEVEL_(0.001), WINDOW_SIZE_(30)
{ }

LucasKanadeOpenCV::LucasKanadeOpenCV(int numPyramidLevels,
                                     int windowSize,
                                     int maxNumFeaturePoints,
                                     double minFeaturePointDist,
                                     double qualityLevel,
                                     double stopCritMaxNumIter,
                                     double stopCritEpsilon) : 
  EPSILON_(stopCritEpsilon),
  MAX_NUM_FEATURE_POINTS_(maxNumFeaturePoints),
  MAX_NUM_ITER_(stopCritMaxNumIter),
  MIN_POINT_DISTANCE_(minFeaturePointDist),
  NUM_PYRAMID_LEVELS_(numPyramidLevels),
  QUALITY_LEVEL_(qualityLevel),
  WINDOW_SIZE_(windowSize)
{ }

void LucasKanadeOpenCV::compute(const CImg< unsigned char > &I1,
                                const CImg< unsigned char > &I2,
                                SparseVectorField &V)

{
  const int W = I1.width();
  const int H = I1.height();
  
  CvPoint2D32f *cvFeaturePoints1;
  CvPoint2D32f *cvFeaturePoints2;
  IplImage *eigImage;
  int numFeaturePoints;
  IplImage *I1_cv;
  IplImage *I2_cv;
  char *status;
  IplImage *tmpImage;
  CvSize cvWindowSize;
  int x1, y1;
  double x2, y2;
  
  I1_cv = cvCreateImageHeader(cvSize(W, H), IPL_DEPTH_8U, 1);
  I2_cv = cvCreateImageHeader(cvSize(W, H), IPL_DEPTH_8U, 1);
  
  cvSetImageData(I1_cv, (void *)I1.data(), W * sizeof(unsigned char));
  cvSetImageData(I2_cv, (void *)I2.data(), W * sizeof(unsigned char));
  
  eigImage = cvCreateImage(cvSize(W, H), IPL_DEPTH_32F, 1);
  tmpImage = cvCreateImage(cvSize(W, H), IPL_DEPTH_32F, 1);
  
  cvFeaturePoints1 = new CvPoint2D32f[W * H];
  cvFeaturePoints2 = new CvPoint2D32f[W * H];
  
  status = new char[W * H];
  
  numFeaturePoints = MAX_NUM_FEATURE_POINTS_;
  cvWindowSize = cvSize(WINDOW_SIZE_, WINDOW_SIZE_);
  
  cvGoodFeaturesToTrack(I1_cv, eigImage, tmpImage,
                        cvFeaturePoints1, &numFeaturePoints,
                        QUALITY_LEVEL_, MIN_POINT_DISTANCE_);
  
  for(int i = 0; i < numFeaturePoints; i++)
  {
    x1 = cvFeaturePoints1[i].x;
    y1 = cvFeaturePoints1[i].y;
    
    V.addVector(Point(x1, y1));
  }
  
  for(int i = 0; i < numFeaturePoints; i++)
  {
    cvFeaturePoints1[i].x = V.getStartPoint(i).x();
    cvFeaturePoints1[i].y = V.getStartPoint(i).y();
  }
  
  cvCalcOpticalFlowPyrLK(I1_cv, I2_cv, NULL, NULL,
                         cvFeaturePoints1, cvFeaturePoints2,
                         numFeaturePoints, cvWindowSize, NUM_PYRAMID_LEVELS_ - 1, status, NULL,
                         cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, MAX_NUM_ITER_, EPSILON_), 0);
  
  for(int i = 0; i < numFeaturePoints; i++)
  {
    x2 = cvFeaturePoints2[i].x;
    y2 = cvFeaturePoints2[i].y;
    
    V.setEndPoint(i, x2, y2);
  }
  
  cvReleaseImageHeader(&I1_cv);
  cvReleaseImageHeader(&I2_cv);
  cvReleaseImage(&eigImage);
  cvReleaseImage(&tmpImage);
  delete[] cvFeaturePoints1;
  delete[] cvFeaturePoints2;
  delete[] status;
}

string LucasKanadeOpenCV::getName() const
{
  return "LucasKanadeOpenCV";
}

bool LucasKanadeOpenCV::isDual() const
{
  return false;
}

void LucasKanadeOpenCV::printInfoText() const
{
  cout<<"Lucas-Kanade optical flow algorithm (OpenCV implementation)"<<endl;
  cout<<"==========================================================="<<endl;
  
  // TODO
  /*cout<<"Input image width: "<<baseWidth<<endl;
  cout<<"Input image height: "<<baseHeight<<endl;*/
}

#endif
