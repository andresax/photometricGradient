#ifndef PHOTOMETRICGRADIENT_H_
#define PHOTOMETRICGRADIENT_H_

#include <opencv2/core/core.hpp>
#include <glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ShaderProgram.h>
#include <OpenGLProgram.h>
#include <photometricGradient/types.hpp>

namespace photometricGradient{
class PhotometricGradient : public OpenGLProgram {
public:
  PhotometricGradient(int imageWidth, int imageHeight, GLFWwindow* window);
  virtual ~PhotometricGradient();


  void initShaders();
  const std::vector<glm::vec3>&  twoImageGradient(const cv::Mat &image1, const cv::Mat &image2, const photometricGradient::CameraType &cam1, const photometricGradient::CameraType &cam2,//
      int numActiveVertices, int levelOfDetail,bool useSSD =false);

  void setVertexBufferObj(GLuint vertexBufferObj) {
    vertexBufferObj_ = vertexBufferObj;
  }

  void setNumActiveVertices(int numActiveVertices) {
    numActiveVertices_ = numActiveVertices;
  }

  void setWindowNcc(int windowNcc) {
    window_NCC_ = windowNcc;
  }

private:


  ShaderProgram *depthMapProgram_;
  ShaderProgram *reprojectionProgram_;
  ShaderProgram *nccProgram_;
  ShaderProgram *nccGradProgram_;
  ShaderProgram *gradFlowProgram_;
  ShaderProgram *gradCollProgram_;
  ShaderProgram *nccCollProgram_;


  GLuint vertexBufferObj_;
  GLuint framebufferDepth_, framebufferDepth2_;

  GLuint depthTexture_, depthTexture2_,image2ReprojTex_;
  GLuint NCCTex_, meanTex_, varTex_, reliabilityTex_;
  GLuint simGradTex_,gradientValuesTexture_;

  int numActiveVertices_;
  int window_NCC_;

};
}
#endif /* REFINEMENT_PHOTOMETRIC_PHOTOMETRICGRADIENT_H_ */
