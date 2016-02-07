
SET(INST_HEADERS "DenseImageExtrapolator.h"
                 "DenseImageMorpher.h"
                 "DenseMotionExtractor.h"
                 "DenseVectorFieldIO.h"
                 "DualDenseMotionExtractor.h"
                 "ForwardDenseImageExtrapolator.h"
                 "HornSchunck.h"
                 "ImageExtrapolatorDriver.h"
                 "ImagePyramid.h"
                 "InverseDenseImageExtrapolator.h"
                 "LucasKanade.h"
                 "LucasKanadeOpenCV.h"
                 "LucasKanadeROI.h"
                 "MotionExtractorDriver.h"
                 "Proesmans.h"
                 "PXMFileUtils.h"
                 "PyramidalDenseMotionExtractor.h"
                 "PyramidalHornSchunck.h"
                 "PyramidalLucasKanade.h"
                 "PyramidalProesmans.h"
                 "ROI.h"
                 "SparseImageExtrapolator.h"
                 "SparseImageMorpher.h"
                 "SparseMotionExtractor.h"
                 "SparseVectorField.h"
                 "SparseVectorFieldIO.h"
                 "VectorFieldIllustrator.h")

SET(SRCS "DenseImageMorpher.cpp"
         "DenseVectorFieldIO.cpp"
         "DualDenseMotionExtractor.cpp"
         "HornSchunck.cpp"
         "ImageExtrapolatorDriver.cpp"
         "ImagePyramid.cpp"
         "InverseDenseImageExtrapolator.cpp"
         "LucasKanade.cpp"
         "LucasKanadeOpenCV.cpp"
         "LucasKanadeROI.cpp"
         "MotionExtractorDriver.cpp"
         "Proesmans.cpp"
         "PXMFileUtils.cpp"
         "PyramidalHornSchunck.cpp"
         "PyramidalDenseMotionExtractor.cpp"
         "PyramidalLucasKanade.cpp"
         "PyramidalProesmans.cpp"
         "ROI.cpp"
         "SparseImageExtrapolator.cpp"
         "SparseImageMorpher.cpp"
         "SparseVectorField.cpp"
         "SparseVectorFieldIO.cpp"
         "VectorFieldIllustrator.cpp")

INCLUDE_DIRECTORIES(.)

ADD_LIBRARY(optflow SHARED ${SRCS})

IF(WITH_CGAL)
  SET(LIBS ${LIBS} ${CGAL_LIBRARY})
ENDIF()

IF(WITH_OPENCV)
  SET(LIBS ${LIBS} ${OpenCV_LIBS})
ENDIF()

TARGET_LINK_LIBRARIES(optflow ${LIBS})

INSTALL(TARGETS optflow LIBRARY DESTINATION lib)

INSTALL(FILES ${INST_HEADERS} DESTINATION include/optflow)
