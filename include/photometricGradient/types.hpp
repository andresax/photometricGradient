#ifndef TYPESPHOTOMETRICGRADIENT_HPP_
#define TYPESPHOTOMETRICGRADIENT_HPP_

#include <glm/glm.hpp>
#include <vector>
#include <string>


namespace photometricGradient{

 
struct CameraType {
  glm::mat3 intrinsics;
  glm::mat3 rotation;
  glm::vec3 translation;
  glm::mat4 cameraMatrix;
  glm::vec3 center;
  glm::mat4 mvp;
  glm::vec3 distortion_coeff;

  std::string pathImage;

  int imageWidth;
  int imageHeight;

  bool valid = true;

  std::vector<int> visiblePoints;
};

}

#endif /* TYPESSEMANTICREC_HPP_ */
