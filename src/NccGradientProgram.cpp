#include <NccGradientProgram.h>

#include <opencv2/imgproc/imgproc.hpp>


namespace photometricGradient{
NccGradientProgram::NccGradientProgram(int imageWidth, int imageHeight) :
    ShaderProgram(imageWidth, imageHeight) {

  lod_=0.0;
}

NccGradientProgram::~NccGradientProgram() {
}

void NccGradientProgram::populateTex(const cv::Mat& image) {

  cv::Mat image1Gray;
  cv::cvtColor(image, image1Gray, CV_RGB2GRAY);
  //image1 texture
  glBindTexture(GL_TEXTURE_2D, image1simGradTex_);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imageWidth_, imageHeight_, 0, GL_RED, GL_UNSIGNED_BYTE, image1Gray.data);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RED, imageWidth_, imageHeight_, GL_RED, GL_UNSIGNED_BYTE, image1Gray.data);

  image1Gray.release();
}

void NccGradientProgram::initializeFramebufAndTex(GLuint& simGradTex) {
  glGenFramebuffers(1, &framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  glGenTextures(1, &simGradTex);
  glBindTexture(GL_TEXTURE_2D, simGradTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, imageWidth_, imageHeight_, 0, GL_RED, GL_FLOAT, nullptr);
  defaultTextureParameters();
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, simGradTex, 0);

  checkFrameBuffer("NccGradientProgram::initializeFramebufAndTex");

  glGenTextures(1, &image1simGradTex_);
  glBindTexture(GL_TEXTURE_2D, image1simGradTex_);
  defaultTextureParameters();
  simGradTex_ = simGradTex;
}

void NccGradientProgram::compute(bool renderFrameBuf) {
  if (renderFrameBuf)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  else
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //glBindFramebuffer(GL_FRAMEBUFFER, 0);

  GLuint attachmentsSim[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, attachmentsSim);

  glViewport(0, 0, imageWidth_, imageHeight_);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shaderManager_.enable();
  glUniform1f(imWsimid_, static_cast<float>(imageWidth_));
  glUniform1f(imHsimid_, static_cast<float>(imageHeight_));
  glUniform1f(wId_, static_cast<float>(window_));
    glUniform1f(lodId_, lod_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, image1simGradTex_);
  glUniform1i(sim_img1_id_, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, image2ReprojTex_);
  glUniform1i(sim_img2_id_, 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, meanTexId_);
  glUniform1i(sim_mean_id_, 2);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, varTexId_);
  glUniform1i(sim_var_id_, 3);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, nccTexId_);
  glUniform1i(sim_ncc_id_, 4);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, reliabTexId_);
  glUniform1i(sim_rel_id_, 5);

  glEnableVertexAttribArray(posAttribsimGradId_);
  glEnableVertexAttribArray(texcoordAttribsimGradId_);

  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
  glVertexAttribPointer(posAttribsimGradId_, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glVertexAttribPointer(texcoordAttribsimGradId_, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const GLvoid*) (2 * sizeof(GLfloat)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBufferObj_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(posAttribsimGradId_);
  glDisableVertexAttribArray(texcoordAttribsimGradId_);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void NccGradientProgram::init() {
  shaderManager_.init();
  shaderManager_.addShader(GL_VERTEX_SHADER, "shaders/sim_grad_vertex_shader.glsl");
  shaderManager_.addShader(GL_FRAGMENT_SHADER, "shaders/sim_grad_fragment_shader.glsl");
  shaderManager_.finalize();
}

void NccGradientProgram::createAttributes() {
  posAttribsimGradId_ = shaderManager_.getAttribLocation("position");
  texcoordAttribsimGradId_ = shaderManager_.getAttribLocation("texcoord");
}

void NccGradientProgram::createUniforms() {
  sim_mean_id_ = shaderManager_.getUniformLocation("mean");
  sim_var_id_ = shaderManager_.getUniformLocation("var");
  sim_ncc_id_ = shaderManager_.getUniformLocation("ncc");
  sim_rel_id_ = shaderManager_.getUniformLocation("reliability");
  sim_img1_id_ = shaderManager_.getUniformLocation("img1");
  sim_img2_id_ = shaderManager_.getUniformLocation("img2");
  imWsimid_ = shaderManager_.getUniformLocation("imW");
  imHsimid_ = shaderManager_.getUniformLocation("imH");
  wId_ = shaderManager_.getUniformLocation("window");
  lodId_ = shaderManager_.getUniformLocation("LOD");
}
}