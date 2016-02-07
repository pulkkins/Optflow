
INCLUDE_DIRECTORIES(../lib)

ADD_EXECUTABLE(extractmotion extractmotion.cpp)
ADD_EXECUTABLE(extrapolate extrapolate.cpp)
ADD_EXECUTABLE(morph morph.cpp)

SET(LIBS "boost_program_options" "optflow" "pthread")

IF(WITH_CGAL)
  SET(LIBS ${LIBS} ${CGAL_LIBRARY})
ENDIF()

IF(WITH_OPENCV)
  SET(LIBS ${LIBS} ${OpenCV_LIBS})
ENDIF()

TARGET_LINK_LIBRARIES(extractmotion ${LIBS})
TARGET_LINK_LIBRARIES(extrapolate ${LIBS})
TARGET_LINK_LIBRARIES(morph ${LIBS})

INSTALL(TARGETS extractmotion extrapolate morph DESTINATION bin)
