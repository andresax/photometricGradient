cmake_minimum_required(VERSION 2.8.3)
#----------------------- CMAKE MODULES ------------------------ 
    
    
# ----------------------- LIBRARIES ---------------------------
find_package(Boost COMPONENTS filesystem system thread REQUIRED)
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
  link_directories(../external/TransforMesh/build)
  MESSAGE( STATUS "CMAKE_BUILD_TYPE:    release->     " ${CMAKE_BUILD_TYPE} )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math  ")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  link_directories(../external/TransforMesh/buildDebug)
  MESSAGE( STATUS "CMAKE_BUILD_TYPE:  debug->      " ${CMAKE_BUILD_TYPE} )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math  ")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math -fsanitize=address ")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "")
  link_directories(../external/TransforMesh/build)
  MESSAGE( STATUS "CMAKE_BUILD_TYPE:  release->      " ${CMAKE_BUILD_TYPE} )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math  ")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math -fsanitize=address ")
endif()

#add_definitions(-DBASE_PATH_SHADERS="")

# ----------------------- INCLUDES ----------------------------
include_directories(  ${OPENGL_INCLUDE_DIR} 
                      ${GLEW_INCLUDE_DIRS}
                      ${GMPXX_INCLUDE_DIR}
                      ${OpenCV_INCLUDE_DIRS} 
                      ${CGAL_INCLUDE_DIRS}
		      ${GLM_INCLUDE_DIRS}/glm
                      ${SOIL_INCLUDE_DIRS}
                      include/photometricGradient/
                      include/
                      ) 

# ------------------------ MESSAGES ----------------------------
 
# ------------------------ BUILD -------------------------------


file(GLOB_RECURSE photometricGradient_SOURCE 
                  src/*.cpp
                  )
file(GLOB_RECURSE photometricGradient_H 
                  ./include/photometricGradient/*.h
                  ./include/
                  ./include/photometricGradient/*.hpp
                  )

add_library(photometricGradient ${photometricGradient_SOURCE} ${photometricGradient_H})
target_link_libraries(photometricGradient ${OpenCV_LIBS} 
                                            ${MPRF_LIBRARIES}
                                            ${GMPXX_LIBRARIES}
                                            ${GMP_LIBRARIES}
                                            ${LAPACK_LIBRARIES}
                                            ${OPENGL_LIBRARIES}
                                            ${GLEW_LIBRARIES}
                                            #${GLFW_LIBRARY}
					    glfw ${GLFW_LIBRARIES}
					    ${GLM_LIBRARIES}
                                            -lmpfr
                                            ${CGAL_LIBRARY}
                                            ${Boost_LIBRARIES})
