cmake_minimum_required(VERSION 2.8.3)
project(photometricGradient)
set( CMAKE_BUILD_TYPE "Debug" )
set( OpenGL_GL_PREFERENCE "GLVND" )
set( CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE) 
set( photometricGradient_BUILD_DIR "${PROJECT_SOURCE_DIR}/build/src/}" PARENT_SCOPE)
set( photometricGradient_LIBRARIES "depthMapProgram depthMapXYZProgram gradientCollectorProgram gradientFlowProgram logger nccGradientProgram nccProgram openGLProgram photometricGradient reprojectionProgram shaderManager shaderProgram textureManager" PARENT_SCOPE)
    
# ----------------------- LIBRARIES ---------------------------
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(CGAL REQUIRED)
find_package(GLEW REQUIRED)  
find_package(OpenCV REQUIRED)
find_package(glm REQUIRED)
find_package(MPFR REQUIRED) 
find_package(OpenMP) 

if (OPENMP_FOUND) 
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# ----------------------- GCC FLAGS ----------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  MESSAGE( STATUS "CMAKE_BUILD_TYPE:    release->     " ${CMAKE_BUILD_TYPE} )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math  ")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  MESSAGE( STATUS "CMAKE_BUILD_TYPE:  debug->      " ${CMAKE_BUILD_TYPE} )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math  ")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "")
  MESSAGE( STATUS "CMAKE_BUILD_TYPE:  release->      " ${CMAKE_BUILD_TYPE} )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math  ")
endif()

# ------------------------ BUILD -------------------------------
add_subdirectory(src)
