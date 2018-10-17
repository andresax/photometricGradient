#ifndef DEPTHMAPXYZPROGRAM_H_
#define DEPTHMAPXYZPROGRAM_H_

#include <ShaderProgram.h>
#include <glm.hpp>


namespace photometricGradient{

class DepthMapXYZProgram : public ShaderProgram {
public:
  DepthMapXYZProgram(int imageWidth, int imageHeight);
  virtual ~DepthMapXYZProgram();
  void initTexAndFramebuf();

  void initializeFramebufAndTex( GLuint& depthTextureId);
  void compute(bool renderFrameBuf=true);

  void setMvp(const glm::mat4& mvp) {
    mvp_ = mvp;
  }

  const glm::vec3& getCenter() const {
    return center_;
  }

  void setCenter(const glm::vec3& center) {
    center_ = center;
  }

  void setDepthTexture(GLuint depthTexture) {
    depthTexture_ = depthTexture;
  }

private:
  void init();

  void createAttributes();
  void createUniforms();
  inline void createTransformFeedback(){};

  GLuint posAttribDepthId_;
  GLuint mvpId_, centerid_;
  GLuint framebuffer_,shadowMap_,depthTexture_;
  GLuint imageReprojTex_;

  glm::mat4 mvp_;
  glm::vec3 center_;
};
}
#endif /* DEPTHMAPPROGRAM_H_ */
