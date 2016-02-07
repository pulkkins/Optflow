/*
 * This program is for running different motion extraction algorithms.
 */

#include "DenseMotionExtractor.h"
#if defined (WITH_OPENCV) && defined(WITH_CGAL)
#include "LucasKanadeOpenCV.h"
#endif
#include "MotionExtractorDriver.h"
#include "PyramidalHornSchunck.h"
#include "PyramidalLucasKanade.h"
#include "PyramidalProesmans.h"
#include "SparseImageExtrapolator.h"
#include "version.h"

#include <boost/program_options.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace boost::program_options;
using namespace std;

int main(int argc, char **argv)
{
  DenseMotionExtractor *denseMotionExtractor = NULL;
#ifdef WITH_CGAL
  SparseMotionExtractor *sparseMotionExtractor = NULL;
#endif
  
  options_description generalArgs("general options");
  generalArgs.add_options()
    ("help,h",    "print usage")
    ("options,o", value< std::string >(), "print options specific to the algorithm <arg>")
    ("version,v", "print version number");
  
  options_description mandatoryArgs("positional arguments");
  mandatoryArgs.add_options()
    ("image1",    value< std::string >()->required(), "first image")
    ("image2",    value< std::string >()->required(), "second image")
    ("algorithm", value< std::string >()->required(), "motion detection algorithm (lucaskanade, opencv, proesmans)")
    ("outprefix", value< std::string >()->required(), "output file prefix");
  
  positional_options_description posArgs;
  posArgs.add("image1", 1);
  posArgs.add("image2", 1);
  posArgs.add("algorithm", 1);
  posArgs.add("outprefix", 1);
  
  options_description optionalArgs("optional arguments");
  optionalArgs.add_options()
    ("numlevels", value< int >(), "number of pyramid levels (default = 4)");
  
  options_description hornSchunckArgs("Options for the Horn&Schunck algorithm");
  hornSchunckArgs.add_options()
    ("numiter",    value< int >(),   "number of Gauss-Seidel/SOR iterations")
    ("alpha",      value< float >(), "smoothness parameters")
    ("relaxcoeff", value< float >(), "SOR relaxation coefficient");
  
  // options specific to the Lucas-Kanade algorithm
  options_description lucasKanadeArgs("Options for the Lucas-Kanade algorithm");
  lucasKanadeArgs.add_options()
    ("windowradius", value< int >(),   "feature matching window radius (default = 16)")
    ("numlsqiter",   value< int >(),   "number of iterations (default = 5)")
    ("tau",          value< float >(), "eigenvalue threshold for feature matching (default = 0.0025)")
    ("sigmap",       value< float >(), "regularization parameter (default = 0)");
  
  // options specific to the Lucas-Kanade algorithm (OpenCV)
  options_description opencvArgs("Options for the Lucas-Kanade algorithm (OpenCV)");
  opencvArgs.add_options()
    ("windowsize",   value< int >(),   "feature matching window size (default = 30)")
    ("maxnumpoints", value< int >(),   "maximum number of feature points (default = 1000)")
    ("minpointdist", value< float >(), "minimum distance between feature points (default = 5)")
    ("qualitylevel", value< float >(), "feature point quality threshold (default = 0.001)")
    ("maxnumiter",   value< int >(),   "maximum number of iterations (default = 10)")
    ("epsilon",      value< float >(), "stopping criterion threshold (default = 0.001)");
  
  // options specific to the Proesmans algorithm
  options_description proesmansArgs("Options for the Proesmans algorithm");
  proesmansArgs.add_options()
    ("numdiffiter", value< int >(),   "number of diffusion iterations (default = 200)")
    ("lambda",      value< float >(), "smoothness parameter (default = 100)")
    ("boundcond",   value< int >(),   "boundary conditions (0 = Dirichlet, 1 = Neumann) (default = 1)");
  
  std::string restrictions = "Restrictions:\n -the source images must be 8-bit grayscale images.";
  
  options_description allArgs("");
  allArgs.add(generalArgs).add(mandatoryArgs).add(hornSchunckArgs).
          add(optionalArgs).add(lucasKanadeArgs).add(proesmansArgs).add(opencvArgs);
  options_description allVisibleArgs("Usage: extractmotion image1 image2 algorithm outprefix [algorithm-specific options]");
  allVisibleArgs.add(generalArgs).add(mandatoryArgs).add(optionalArgs);
  
  try {
    variables_map vm;
    command_line_parser cp = command_line_parser(argc, argv);
    cp.options(allArgs);
    cp.positional(posArgs);
    store(cp.run(), vm);
    
    if(vm.size() == 1 && vm.count("help"))
    {
      std::cout<<allVisibleArgs<<std::endl;
      std::cout<<restrictions<<std::endl;
      return EXIT_SUCCESS;
    }
    else if(vm.size() == 1 && vm.count("version"))
    {
      std::cout<<OPTFLOW_VERSION_INFO<<std::endl;
      return EXIT_SUCCESS;
    }
    else if(vm.size() == 1 && vm.count("options") && vm["options"].as< string >() == "hornschunck")
    {
      std::cout<<hornSchunckArgs<<std::endl;
      return EXIT_SUCCESS;
    }
    else if(vm.size() == 1 && vm.count("options") && vm["options"].as< string >() == "lucaskanade")
    {
      std::cout<<lucasKanadeArgs<<std::endl;
      return EXIT_SUCCESS;
    }
    else if(vm.size() == 1 && vm.count("options") && vm["options"].as< string >() == "opencv")
    {
      std::cout<<opencvArgs<<std::endl;
      return EXIT_SUCCESS;
    }
    else if(vm.size() == 1 && vm.count("options") && vm["options"].as< string >() == "proesmans")
    {
      std::cout<<proesmansArgs<<std::endl;
      return EXIT_SUCCESS;
    }
    else if(vm.count("options"))
    {
      std::cout<<"Invalid algorithm name."<<std::endl;
      return EXIT_SUCCESS;
    }
    /*else if(!vm.count("image1") || !vm.count("image2") || 
            !vm.count("algorithm") || !vm.count("outprefix"))
    {
      std::cout<<"One or more required arguments missing."<<std::endl;
      std::cout<<mandatoryArgs<<std::endl;
      return EXIT_FAILURE;
    }*/
    
    vm.notify();
    
    if(vm["algorithm"].as< string >() == "hornschunck")
    {
      HornSchunck::BoundaryConditions boundCond;
      
      int bc = 1;
      if(vm.count("boundcond") > 0)
        bc = vm["boundcond"].as< int >();
      if(bc == 0)
        boundCond = HornSchunck::DIRICHLET;
      else
        boundCond = HornSchunck::NEUMANN;
      
      denseMotionExtractor = new PyramidalHornSchunck(
        vm.count("numiter") > 0    ? vm["numiter"].as< int >() : 500,
        vm.count("alpha") > 0      ? vm["alpha"].as< float >() : 0.7,
        vm.count("relaxcoeff") > 0 ? vm["relaxcoeff"].as< float >() : 1.9,
        vm.count("numlevels") > 0  ? vm["numlevels"].as< int >() : 4,
        boundCond);
    }
    else if(vm["algorithm"].as< string >() == "lucaskanade")
    {
      denseMotionExtractor = new PyramidalLucasKanade(
        vm.count("windowradius") > 0 ? vm["windowradius"].as< int >() : 16,
        vm.count("numlsqiter") > 0   ? vm["numlsqiter"].as< int >() : 5,
        vm.count("tau") > 0          ? vm["tau"].as< float >() : 0.0025,
        vm.count("sigmap") > 0       ? vm["sigmap"].as< float >() : 0.0,
        vm.count("numlevels") > 0    ? vm["numlevels"].as< int >() : 4,
        true);
    }
#if defined (WITH_OPENCV) && defined(WITH_CGAL)
    else if(vm["algorithm"].as< string >() == "opencv")
    {
      sparseMotionExtractor = new LucasKanadeOpenCV(
        vm.count("numlevels") > 0    ? vm["numlevels"].as< int >() : 4,
        vm.count("windowsize") > 0   ? vm["windowsize"].as< int >() : 30,
        vm.count("maxnumpoints") > 0 ? vm["maxnumpoints"].as< int >() : 1000,
        vm.count("minpointdist") > 0 ? vm["minpointdist"].as< float >() : 5,
        vm.count("qualitylevel") > 0 ? vm["qualitylevel"].as< float >() : 0.001,
        vm.count("maxnumiter") > 0   ? vm["maxnumiter"].as< int >() : 10,
        vm.count("epsilon") > 0      ? vm["epsilon"].as< float >() : 0.001);
    }
#endif
    else if(vm["algorithm"].as< string >() == "proesmans")
    {
      Proesmans::BoundaryConditions boundCond;
      
      int bc = 1;
      if(vm.count("boundcond") > 0)
        bc = vm["boundcond"].as< int >();
      if(bc == 0)
        boundCond = Proesmans::DIRICHLET;
      else
        boundCond = Proesmans::NEUMANN;
      
      denseMotionExtractor = new PyramidalProesmans(
        vm.count("numdiffiter") > 0 ? vm["numdiffiter"].as< int >() : 200,
        vm.count("lambda") > 0      ? vm["lambda"].as< float >() : 100.0,
        vm.count("numlevels") > 0   ? vm["numlevels"].as< int >() : 4,
        boundCond);
    }
    else
    {
      std::cout<<"Invalid algorithm name."<<std::endl;
      return EXIT_FAILURE;
    }
  
    std::string srcImgFileName1 = vm["image1"].as< string >();
    std::string srcImgFileName2 = vm["image2"].as< string >();
    std::string outFilePrefix   = vm["outprefix"].as< string >();
    
    if(denseMotionExtractor != NULL)
    {
      MotionExtractorDriver::runDenseMotionExtractor(
        *denseMotionExtractor, srcImgFileName1, srcImgFileName2, outFilePrefix);
      delete denseMotionExtractor;
    }
#ifdef WITH_CGAL
    else if(sparseMotionExtractor != NULL)
    {
      MotionExtractorDriver::runSparseMotionExtractor(
        *sparseMotionExtractor, srcImgFileName1, srcImgFileName2, outFilePrefix);
      delete sparseMotionExtractor;
    }
#endif
  }
  catch(CImgIOException &e1) {
    std::cout<<"Invalid source image(s)."<<std::endl;
    return EXIT_FAILURE;
  }
  catch(required_option &e2) {
    std::cout<<"One or more required arguments missing."<<std::endl;
    std::cout<<allVisibleArgs<<std::endl;
    std::cout<<restrictions<<std::endl;
    return EXIT_FAILURE;
  }
  catch(std::exception &e3) {
    std::cout<<e3.what()<<std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
