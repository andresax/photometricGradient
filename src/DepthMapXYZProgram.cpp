#include <DepthMapXYZProgram.h>

#ifndef BASE_PATH_SHADERS
#define BASE_PATH_SHADERS  "photometricGradient/"
#endif

namespace photometricGradient {
DepthMapXYZProgram::DepthMapXYZProgram(int imageWidth, int imageHeight) :
    ShaderProgram(imageWidth, imageHeight) {
  posAttribDepthId_ = -1;
  mvpId_ = -1;
  depthTexture_ = centerid_ = shadowMap_ = framebuffer_ = imageReprojTex_ = -1;
}

DepthMapXYZProgram::~DepthMapXYZProgram() {
}

void DepthMapXYZProgram::initializeFramebufAndTex(GLuint& depthTextureId) {

  glGenFramebuffers(1, &framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  glGenTextures(1, &depthTextureId);
  glBindTexture(GL_TEXTURE_2D, depthTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, imageWidth_, imageHeight_, 0, GL_RED, GL_FLOAT, nullptr);
  defaultTextureParameters();
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, depthTextureId, 0);
  GLuint tmpdeptht;
  initDepthTex(tmpdeptht);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tmpdeptht, 0);
  imageReprojTex_ = depthTextureId;

  checkFrameBuffer("DepthMapXYZProgram::initializeFramebufAndTex");

}

void DepthMapXYZProgram::compute(bool renderFrameBuf) {

  if (renderFrameBuf) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glViewport(0, 0, imageWidth_, imageHeight_);
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, imageWidth_, imageHeight_);
  }

  glViewport(0, 0, imageWidth_, imageHeight_);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shaderManager_.enable();

  glUniformMatrix4fv(mvpId_, 1, GL_FALSE, &mvp_[0][0]);
  glUniform3fv(centerid_, 1, &center_[0]);

  glEnableVertexAttribArray(posAttribDepthId_);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
  glVertexAttribPointer(posAttribDepthId_, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, depthTexture_);
  glUniform1i(shadowMap_, 4);

  if (useElements_Indices) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBufferObj_);
    glDrawElements(GL_TRIANGLES, numElements_, GL_UNSIGNED_INT, 0);

  } else {
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
    glDrawArrays(GL_TRIANGLES, 0, sizeArray_);
  }

  glDisableVertexAttribArray(posAttribDepthId_);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthMapXYZProgram::init() {
  shaderManager_.init();
  shaderManager_.addShader(GL_VERTEX_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/depthXYZ_vertex_shader.glsl");
  shaderManager_.addShader(GL_GEOMETRY_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/depthXYZ_geometry_shader.glsl");
  shaderManager_.addShader(GL_FRAGMENT_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/depthXYZ_fragment_shader.glsl");
  shaderManager_.finalize();
}

void DepthMapXYZProgram::createAttributes() {
  posAttribDepthId_ = shaderManager_.getAttribLocation("position");
}
void DepthMapXYZProgram::createUniforms() {
  mvpId_ = shaderManager_.getUniformLocation("MVP");
  shadowMap_ = shaderManager_.getUniformLocation("shadowMap");
  centerid_ = shaderManager_.getUniformLocation("center");
}

}
