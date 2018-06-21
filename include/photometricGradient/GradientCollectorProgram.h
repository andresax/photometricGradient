#ifndef GRADIENTCOLLECTORPROGRAM_H_
#define GRADIENTCOLLECTORPROGRAM_H_

#include <ShaderProgram.h>

#include <glm.hpp>

namespace photometricGradient{
class GradientCollectorProgram : public ShaderProgram {
public:
  GradientCollectorProgram(int imageWidth, int imageHeight);
  virtual ~GradientCollectorProgram();

  void compute(bool show =false);
  void createTransformFeedback(int length);
  void resetTransformFeedback(int length);

  void setFeedbackLength(GLuint feedbackLength) {
    feedbackLength_ = feedbackLength;
  }

  const std::vector<glm::vec3>& getFeedbackTr() const {
    return feedbackTr;
  }

  void setGradientValuesTexture(GLuint gradientValuesTexture) {
    gradientValuesTexture_ = gradientValuesTexture;
  }

  void setDepthTexture(GLuint depthTexture) {
    depthTexture_ = depthTexture;
  }

  void setDepthTexture2(GLuint depthTexture2) {
    depthTexture2_ = depthTexture2;
  }

  void setMvp1(const glm::mat4& mvp1) {
    this->mvp1 = mvp1;
  }

  void setMvp2(const glm::mat4& mvp2) {
    this->mvp2 = mvp2;
  }

  void setLod(const float lod) {
    this->lod_ = lod;
  }
  void setCamCenter1(const glm::vec3& camCenter) {
    camCenter1_ = camCenter;
  }
  void setCamCenter2(const glm::vec3& camCenter) {
    camCenter2_ = camCenter;
  }
private:
  void init();

  void createAttributes();
  void createUniforms();

  GLuint posAttrib_, gradientValuesTexture_;

  GLuint feedbackBuffer_;
  GLuint feedbackLength_;
  std::vector<glm::vec3> feedbackTr;
  GLuint mvp1CollectorID_, gradId_, imWCollectorid_, imHCollectorid_, depthTexture_, depthTexture2_;
  GLuint mvpCameraMovingColl_, mvp1CollID_, mvp2CollID_, shadowMapIdColl_, shadowMapId2Coll_,lodId_;
  GLint posAttribCollId_,camCenter1ID_,camCenter2ID_;
  float lod_;
  glm::vec3 camCenter1_,camCenter2_;
  glm::mat4 mvp1, mvp2;
};
}
#endif /* SHADER_MANAGERS_GRADIENTCOLLECTORPROGRAM_H_ */
