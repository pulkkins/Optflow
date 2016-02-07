/*
 * This program is for running motion-based image warping algorithms.
 */

#include "DenseVectorFieldIO.h"
#include "ImageExtrapolatorDriver.h"
#include "InverseDenseImageExtrapolator.h"
#include "SparseImageExtrapolator.h"
#include "SparseVectorFieldIO.h"
#include "version.h"

#include <boost/program_options.hpp>
#include "CImg_config.h"
#include <CImg.h>
#include <cstdlib>
#include <iostream>

using namespace boost::program_options;
using namespace cimg_library;

int main(int argc, char **argv)
{
  InverseDenseImageExtrapolator *denseExtrapolator = NULL;
  CImg< double > *Vd = NULL;
#if defined (WITH_OPENCV) && defined(WITH_CGAL)
  SparseImageExtrapolator *sparseExtrapolator = NULL;
  SparseVectorField *Vs = NULL;
#endif
  CImg< unsigned char > I0;
  
  options_description generalArgs("general options");
  generalArgs.add_options()
    ("help,h",    "print usage")
    ("version,v", "print version number");
  
  options_description reqArgs("positional arguments");
  reqArgs.add_options()
    ("image",        value< std::string >()->required(), "image to extrapolate")
    ("motionfield",  value< std::string >()->required(), "motion field to use")
    ("numtimesteps", value< int >()->required(),         "number of images to extrapolate")
    ("outprefix",    value< std::string >()->required(), "output file prefix");
  
  positional_options_description posArgs;
  posArgs.add("image", 1);
  posArgs.add("motionfield", 1);
  posArgs.add("numtimesteps", 1);
  posArgs.add("outprefix", 1);
  
  options_description allArgs("Usage: extrapolate image motionfield numtimesteps outprefix");
  allArgs.add(generalArgs).add(reqArgs);
  
  std::string notes = "NOTE: If you use a dense motion field (.pdvm), it is assumed to be an inverse motion field (image2->image1).";
  
  try {
    variables_map vm;
    //store(parse_command_line(argc, argv, allArgs), vm);
    command_line_parser cp = command_line_parser(argc, argv);
    cp.options(allArgs);
    cp.positional(posArgs);
    store(cp.run(), vm);
    //notify(vm);
    
    if(vm.size() == 1 && vm.count("help"))
    {
      std::cout<<allArgs<<std::endl;
      std::cout<<notes<<std::endl;
      return EXIT_SUCCESS;
    }
    else if(vm.count("version"))
    {
      std::cout<<OPTFLOW_VERSION_INFO<<std::endl;
      return EXIT_SUCCESS;
    }
    /*else if(!vm.count("image") || !vm.count("motionfield") || 
            !vm.count("numtimesteps") || !vm.count("outprefix"))
    {
      std::cout<<"One or more required arguments missing."<<std::endl;
      std::cout<<reqArgs<<std::endl;
    }*/
    
    vm.notify();
    
    std::string imageFileName       = vm["image"].as< std::string >();
    std::string motionFieldFileName = vm["motionfield"].as< std::string >();
    int numTimeSteps                = vm["numtimesteps"].as< int >();
    std::string outPrefix           = vm["outprefix"].as< std::string >();
    
    I0              = CImg< unsigned char >(imageFileName.c_str());
    std::string ext = motionFieldFileName.substr(motionFieldFileName.size() - 4, 4);
    if(ext == "pdvm")
    {
      Vd = new CImg< double >();
      DenseVectorFieldIO::readVectorField(motionFieldFileName, *Vd);
      denseExtrapolator = new InverseDenseImageExtrapolator();
    }
#if defined (WITH_OPENCV) && defined(WITH_CGAL)
    else if(ext == "psvm")
    {
      Vs = new SparseVectorField();
      SparseVectorFieldIO::readVectorField(motionFieldFileName, *Vs);
      Vs->triangulate();
      sparseExtrapolator = new SparseImageExtrapolator();
    }
#endif
    else
    {
      std::cout<<"Unknown motion field file extension."<<std::endl;
      return EXIT_FAILURE;
    }
    
#ifdef WITH_CGAL
    if(sparseExtrapolator != NULL)
      ImageExtrapolatorDriver::runSparseImageExtrapolator(
        *sparseExtrapolator, I0, *Vs, numTimeSteps, outPrefix);
    else
#endif
      ImageExtrapolatorDriver::runDenseImageExtrapolator(
        *denseExtrapolator, I0, *Vd, numTimeSteps, outPrefix);
  }
  catch(CImgIOException &e1) {
    std::cout<<"Invalid source image(s)."<<std::endl;
    return EXIT_FAILURE;
  }
  catch(required_option &e2) {
    std::cout<<"One or more required arguments missing."<<std::endl;
    std::cout<<allArgs<<std::endl;
    std::cout<<notes<<std::endl;
    return EXIT_FAILURE;
  }
  catch(std::exception &e3) {
    /*std::cout<<allArgs<<std::endl;
    std::cout<<notes<<std::endl;*/
    std::cout<<e3.what()<<std::endl;
    return EXIT_FAILURE;
  }
  
  delete denseExtrapolator;
  delete Vd;
#if defined (WITH_OPENCV) && defined(WITH_CGAL)
  delete sparseExtrapolator;
  delete Vs;
#endif
  
  return EXIT_SUCCESS;
}
