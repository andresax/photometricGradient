#ifndef REPROJECTIONPROGRAM_H_
#define REPROJECTIONPROGRAM_H_

#include <ShaderProgram.h>
#include <glm.hpp>

namespace photometricGradient{
class ReprojectionProgram : public ShaderProgram {
public:
  ReprojectionProgram(int imageWidth, int imageHeight);
  virtual ~ReprojectionProgram();
  void initTex();
    void populateTex(const cv::Mat &image);

    void initializeFramebufAndTex(GLuint &imageReprojTex);

    void compute(bool renderFrameBuf = false);

    void setDepthTexture(GLuint depthTexture, GLuint depthTexture2) {
      depthTexture_ = depthTexture;
      depthTexture2_ = depthTexture2;
    }


    void setMvp(const glm::mat4& mvp1, const glm::mat4& mvp2) {
      mvp1_ = mvp1;
      mvp2_ = mvp2;
    }

    void setCamCenter(const glm::vec3& camCenter) {
      camCenter_ = camCenter;
    }

  void setLod(const float lod) {
    lod_ = lod;
  }
  
  private:
    void init();

    void createAttributes();
    void createUniforms();

    GLuint framebufferReproj_;
    GLuint imageReprojTex_;
    GLuint imageTex_;

    GLuint posAttribReprojId_, mvp1IDReproj_, mvp2IDReproj_, image2TexId_, shadowMap1IdReproj_, shadowMap2IdReproj_;

    GLuint depthTexture_, depthTexture2_, camCenterID_,lodId_;
    glm::vec3 camCenter_;
    glm::mat4 mvp1_, mvp2_;
    float lod_;

};
}
#endif /* SHADER_MANAGERS_REPROJECTIONPROGRAM_H_ */
