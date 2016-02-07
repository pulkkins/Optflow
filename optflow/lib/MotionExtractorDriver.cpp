
#include "CImg_config.h"
#include <CImg.h>
#include <sstream>
#include <string>

#include "DenseVectorFieldIO.h"
#include "DualDenseMotionExtractor.h"
#include "ImageExtrapolatorDriver.h"
#include "MotionExtractorDriver.h"
#include "SparseVectorFieldIO.h"
#include "VectorFieldIllustrator.h"

using namespace cimg_library;

namespace MotionExtractorDriver
{
  static string getBaseName_(const string &fileName)
  {
    size_t last = fileName.find_last_of(".");
    if(last != string::npos)
      return fileName.substr(0, last);
    else
      return fileName;
  }
  
  static void preProcess_(const CImg< unsigned char > &I1,
                          const CImg< unsigned char > &I2,
                          CImg< unsigned char > &I1_smoothed,
                          CImg< unsigned char > &I2_smoothed,
                          CImg< unsigned char > &motionImageF,
                          CImg< unsigned char > *motionImageB = NULL)
  {
    I1_smoothed = I1.get_channel(0);
    I2_smoothed = I2.get_channel(0);
    
    motionImageF.get_shared_channel(0) = I1_smoothed * 0.75;
    motionImageF.get_shared_channel(1) = I1_smoothed * 0.75;
    motionImageF.get_shared_channel(2) = I1_smoothed * 0.75;
    
    if(motionImageB != NULL)
    {
      motionImageB->get_shared_channel(0) = I1_smoothed * 0.75;
      motionImageB->get_shared_channel(1) = I1_smoothed * 0.75;
      motionImageB->get_shared_channel(2) = I1_smoothed * 0.75;
    }
    
    I1_smoothed.blur(3.0f);
    I2_smoothed.blur(3.0f);
  }
  
  static void saveResultImages_(const string &srcFileName1,
                                const string &srcFileName2,
                                const string &outFilePrefix,
                                const CImg< unsigned char > &I1_smoothed,
                                const CImg< unsigned char > &I2_smoothed,
                                const CImg< unsigned char > &motionImageF,
                                const CImg< unsigned char > *motionImageB = NULL)
  {
    string smoothedFileName1 = getBaseName_(srcFileName1.substr(srcFileName1.find_last_of(
      "/")+1, string::npos)) + "-smoothed.png";
    I1_smoothed.save_png(smoothedFileName1.c_str());
    string smoothedFileName2 = getBaseName_(srcFileName2.substr(srcFileName1.find_last_of(
      "/")+1, string::npos)) + "-smoothed.png";
    I2_smoothed.save_png(smoothedFileName2.c_str());
    
    if(motionImageB == NULL)
      motionImageF.save_png((outFilePrefix + "-motion.png").c_str());
    else
    {
      motionImageF.save_png((outFilePrefix + "-motionF.png").c_str());
      motionImageB->save_png((outFilePrefix + "-motionB.png").c_str());
    }
  }
  
  static void saveResultMotionField_(const CImg< double > &VF,
                                     const string &outFilePrefix,
                                     const CImg< double > *VB = NULL)
  {
    if(VB == NULL)
      DenseVectorFieldIO::writeVectorField(VF, outFilePrefix + "-motion.pdvm");
    else
    {
      DenseVectorFieldIO::writeVectorField(VF, outFilePrefix + "-motionF.pdvm");
      DenseVectorFieldIO::writeVectorField(*VB, outFilePrefix + "-motionB.pdvm");
    }
  }
  
#ifdef WITH_CGAL
  static void saveResultMotionField_(const SparseVectorField &V,
                                     const string &outFilePrefix)
  {
    SparseVectorFieldIO::writeVectorField(V, outFilePrefix + "-motion.psvm");
  }
#endif
  
  void runDenseMotionExtractor(DenseMotionExtractor &e,
                               const string &src1,
                               const string &src2,
                               const string &outFilePrefix)
  {
    CImg< unsigned char > I1(src1.c_str());
    CImg< unsigned char > I2(src2.c_str());
    const int W = I1.width();
    const int H = I1.height();
    CImg< unsigned char > I1_smoothed;
    CImg< unsigned char > I2_smoothed;
    CImg< unsigned char > motionImageF(W, H, 1, 3);
    CImg< unsigned char > motionImageB;
    CImg< double > VF, VB;
    
    if(!e.isDual())
    {
      preProcess_(I1, I2, I1_smoothed, I2_smoothed, motionImageF);
      e.compute(I1_smoothed, I2_smoothed, VF);
      VectorFieldIllustrator::renderDenseVectorField(VF, motionImageF);
    }
    else
    {
      motionImageB = CImg< unsigned char >(W, H, 1, 3);
      preProcess_(I1, I2, I1_smoothed, I2_smoothed, motionImageF, &motionImageB);
      e.compute(I1_smoothed, I2_smoothed, VF, VB);
      VectorFieldIllustrator::renderDenseVectorField(VF, motionImageF);
      VectorFieldIllustrator::renderDenseVectorField(VB, motionImageB);
    }
    
    for(int i = 0; i < e.getNumResultQualityChannels(); i++)
    {
      ostringstream ostr;
      ostr<<(i+1);
      CImg< double > V_equalized = 
        VF.get_shared_channel(2 + i).get_equalize(256).normalize(0.0, 255.0);
      V_equalized.save_png((outFilePrefix + "-quality" + ostr.str() + ".png").c_str());
    }
    
    if(!e.isDual())
    {
      saveResultImages_(src1, src2, outFilePrefix, I1_smoothed, I2_smoothed, motionImageF);
      saveResultMotionField_(VF, outFilePrefix);
    }
    else
    {
      saveResultImages_(src1, src2, outFilePrefix, I1_smoothed, I2_smoothed, motionImageF, &motionImageB);
      saveResultMotionField_(VF, outFilePrefix, &VB);
    }
  }

#ifdef WITH_CGAL
  void runSparseMotionExtractor(SparseMotionExtractor &e,
                                const string &src1,
                                const string &src2,
                                const string &outFilePrefix)
  {
    CImg< unsigned char > I1(src1.c_str());
    CImg< unsigned char > I2(src2.c_str());
    const int W = I1.width();
    const int H = I1.height();
    CImg< unsigned char > I1_smoothed;
    CImg< unsigned char > I2_smoothed;
    CImg< unsigned char > motionImage(W, H, 1, 3);
    SparseVectorField V;
    
    preProcess_(I1, I2, I1_smoothed, I2_smoothed, motionImage);
    
    e.compute(I1_smoothed, I2_smoothed, V);
    V.triangulate();
    VectorFieldIllustrator::renderSparseVectorField(V, motionImage);
    
    saveResultImages_(src1, src2, outFilePrefix, I1_smoothed, I2_smoothed, motionImage);
    saveResultMotionField_(V, outFilePrefix);
  }
#endif
}
