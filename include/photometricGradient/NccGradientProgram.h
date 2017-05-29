#ifndef NCCGRADIENTPROGRAM_H_
#define NCCGRADIENTPROGRAM_H_

#include <ShaderProgram.h>


namespace photometricGradient{
class NccGradientProgram : public ShaderProgram {
public:
  NccGradientProgram(int imageWidth, int imageHeight);
  virtual ~NccGradientProgram();

  void populateTex(const cv::Mat &image);
  void initializeFramebufAndTex(GLuint& simGradTex);

  void compute(bool renderFrameBuf = false);

  void setImage2ReprojTex(GLuint image2ReprojTex) {
    image2ReprojTex_ = image2ReprojTex;
  }

  void setWindow(int window) {
    window_ = window;
  }
  void setLod(float lod) {
    lod_ = lod;
  }

  void setImageTex(GLuint imageTex) {
    imageTex_ = imageTex;
  }

  void setMeanTexId(GLuint meanTexId) {
    meanTexId_ = meanTexId;
  }

  void setNccTexId(GLuint nccTexId) {
    nccTexId_ = nccTexId;
  }

  void setReliabTexId(GLuint reliabTexId) {
    reliabTexId_ = reliabTexId;
  }

  void setVarTexId(GLuint varTexId) {
    varTexId_ = varTexId;
  }

private:

  void init();

  void createAttributes();
  void createUniforms();

  GLuint framebuffer_;
  /*uniforms id*/
  GLuint image2ReprojTexId_, imageTexId_, wId_, simGradTex_,image1simGradTex_;
  /*tex id*/
  GLuint nccTexId_, image2ReprojTex_, imageTex_, varTexId_, meanTexId_, reliabTexId_;
  /*attributes id*/
  GLuint posId_, texCoordId_,lodId_;

  //uniforms ID shaderSimilarityGrad_ shader
  GLint sim_mean_id_, sim_var_id_, sim_ncc_id_,sim_rel_id_, sim_img1_id_, sim_img2_id_;
  //attribute ID simGrad shader
  GLint posAttribsimGradId_, texcoordAttribsimGradId_, imWsimid_, imHsimid_;
  int window_;
  float lod_;
};
}
#endif /* NCCGRADIENTPROGRAM_H_ */
