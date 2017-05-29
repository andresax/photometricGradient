#ifndef GRADIENTFLOWPROGRAM_H_
#define GRADIENTFLOWPROGRAM_H_

#include <ShaderProgram.h>

#include <glm.hpp>

namespace photometricGradient{
class GradientFlowProgram : public ShaderProgram {
public:
  GradientFlowProgram(int imageWidth, int imageHeight);
  virtual ~GradientFlowProgram();

  void populateTex(const cv::Mat &image1, const cv::Mat &image2);
  void initializeFramebufAndTex(GLuint& gradTex);

  void compute(bool renderFrameBuf = false);

  void setCam1toPoint(const glm::mat4& cam1toPoint) {
    this->cam1toPoint = cam1toPoint;
  }

  void setDepthTexture(GLuint depthTexture) {
    depthTexture_ = depthTexture;
  }

  void setDepthTexture2(GLuint depthTexture2) {
    depthTexture2_ = depthTexture2;
  }

  void setMvp1(const glm::mat4& mvp1) {
    mvp1_ = mvp1;
  }

  void setMvp1Orig(const glm::mat4& mvp1Orig) {
    mvp1Orig_ = mvp1Orig;
  }

  void setMvp2(const glm::mat4& mvp2) {
    mvp2_ = mvp2;
  }

  void setMvp2Orig(const glm::mat4& mvp2Orig) {
    mvp2Orig_ = mvp2Orig;
  }

  void setT(const glm::vec3& t) {
    t_ = t;
  }

  void setT2(const glm::vec3& t2) {
    t2_ = t2;
  }
  void setLod(const float lod) {
    lod_ = lod;
  }

  void setSimGradTex(GLuint simGradTex) {
    simGradTex_ = simGradTex;
  }

private:

  void init();

  void createAttributes();
  void createUniforms();

  GLuint framebuffer_;
  GLuint gradSimilarityImgId_, image1Id_, shadowMapId_, shadowMapId2_, gradimage2yId_, gradimage2xId_, pointToCamID_;
  GLuint idFacetId_, gradientContributionId_, mvp1ID_, mvp2ID_, mvp1OrigID_, mvp2OrigID_, cam1PositionId_, cam2PositionId_, mvpCameraMoving_,gradTex_;

  GLuint imageTex_, textureObjX_, textureObjY_, depthTexture_, depthTexture2_,simGradTex_;
  GLint posAttribId_,lodId_;
  GLfloat lod_;

  glm::vec3 t_, t2_;
  glm::mat4 mvp1_, mvp2_, mvp1Orig_, mvp2Orig_, cam1toPoint;
};
}
#endif /* SHADER_MANAGERS_GRADIENTFLOWPROGRAM_H_ */
