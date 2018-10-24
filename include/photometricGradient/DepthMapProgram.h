#ifndef DEPTHMAPPROGRAM_H_
#define DEPTHMAPPROGRAM_H_

#include "ShaderProgram.h"
#include <glm/glm.hpp>


namespace photometricGradient{

class DepthMapProgram : public ShaderProgram {
public:
  DepthMapProgram(int imageWidth, int imageHeight);
  virtual ~DepthMapProgram();
  void initTexAndFramebuf();

  void initializeFramebufAndTex(GLuint& framebufferDepth, GLuint& depthTextureId);
  void computeDepthMap(const GLuint &framebufferDepth, const glm::mat4 &mvp);

  void setMvp(const glm::mat4& mvp) {
    mvp_ = mvp;
  }

private:
  void init();

  void createAttributes();
  void createUniforms();
  inline void createTransformFeedback(){};

  GLuint posAttribDepthId_;
  GLuint mvpId_;

  glm::mat4 mvp_;
};
}
#endif /* DEPTHMAPPROGRAM_H_ */
