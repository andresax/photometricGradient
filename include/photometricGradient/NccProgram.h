#ifndef NCCPROGRAM_H_
#define NCCPROGRAM_H_

#include <ShaderProgram.h>

#include <opencv2/core/core.hpp>
#include <glm.hpp>

namespace photometricGradient{
class NccProgram  : public ShaderProgram {
public:
  NccProgram(int imageWidth, int imageHeight);
  virtual ~NccProgram();
  void initTex();
  void populateTex(const cv::Mat &image);
  void initializeFramebufAndTex(GLuint &nccTexId, GLuint &varTexId, GLuint &meanTexId, GLuint &reliabilityId);

  void compute(bool renderFrameBuf = false);

  void setImage2ReprojTex(GLuint image2ReprojTex) {
    image2ReprojTex_ = image2ReprojTex;
  }

  void setWindow(int window) {
    window_ = window;
  }

  void setLod(int lod) {
    lod_ = lod;
  }

  void setDepthTexture(GLuint depthTexture) {
    depthTexture_ = depthTexture;
  }

  void setDepthXYZ(GLuint depthXYZ) {
    depthXYZ_ = depthXYZ;
  }


private:
  void init();

  void createAttributes();
  void createUniforms();

  GLuint framebuffer_, depthTexture_,shadowMapId_;
  /*uniforms id*/
  GLuint imWid_, imHid_, image2ReprojTexId_, depthXYZId_, imageTexId_, wId_,lodId_;
  /*tex id*/
  GLuint nccTexId_, image2ReprojTex_,depthXYZ_, imageTex_, varTexId_, meanTexId_, reliabTexId_;
  /*attributes id*/
  GLuint posId_, texCoordId_;

  int window_;
  float lod_;
};
}
#endif /* SHADER_MANAGERS_NCCPROGRAM_H_ */
