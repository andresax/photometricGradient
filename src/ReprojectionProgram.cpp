#include <ReprojectionProgram.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#ifndef BASE_PATH_SHADERS
#define BASE_PATH_SHADERS  "photometricGradient/"
#endif

namespace photometricGradient {
ReprojectionProgram::ReprojectionProgram(int imageWidth, int imageHeight) :
    ShaderProgram(imageWidth, imageHeight) {
  framebufferReproj_ = imageReprojTex_ = imageTex_ = -1;
  posAttribReprojId_ = mvp1IDReproj_ = mvp2IDReproj_ = image2TexId_ = shadowMap1IdReproj_ = shadowMap2IdReproj_ = -1;
  depthTexture_ = depthTexture2_ = camCenter1ID_ = camCenter2ID_ =  lodId_ = -1;
  lod_ = 0;
}

void ReprojectionProgram::initTex() {
  glGenTextures(1, &imageTex_);
  glBindTexture(GL_TEXTURE_2D, imageTex_);
  defaultTextureParameters();
}

void ReprojectionProgram::populateTex(const cv::Mat& image) {
  cv::Mat image2Gray;
  if (image.channels() > 1) {
    cv::cvtColor(image, image2Gray, cv::COLOR_RGB2GRAY);
  } else {
    image2Gray = image;
  }
  glBindTexture(GL_TEXTURE_2D, imageTex_);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imageWidth_, imageHeight_, 0, GL_RED, GL_UNSIGNED_BYTE, image2Gray.data);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RED, imageWidth_, imageHeight_, GL_RED, GL_UNSIGNED_BYTE, image2Gray.data);
  image2Gray.release();
}

void ReprojectionProgram::compute(bool renderFrameBuf) {

  if (renderFrameBuf) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferReproj_);
    glViewport(0, 0, imageWidth_, imageHeight_);
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, imageWidth_, imageHeight_);
  }

  GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT };
  glDrawBuffers(2, attachments);

  glViewport(0, 0, imageWidth_, imageHeight_);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  shaderManager_.enable();

  glUniformMatrix4fv(mvp1IDReproj_, 1, GL_FALSE, &mvp1_[0][0]);
  glUniformMatrix4fv(mvp2IDReproj_, 1, GL_FALSE, &mvp2_[0][0]);
  glUniform1f(lodId_, lod_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthTexture_);
  glUniform1i(shadowMap1IdReproj_, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, depthTexture2_);
  glUniform1i(shadowMap2IdReproj_, 1);
  glUniform3fv(camCenter1ID_, 1, &camCenter1_[0]);
  glUniform3fv(camCenter2ID_, 1, &camCenter2_[0]);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, imageTex_);
  glUniform1i(image2TexId_, 2);

  glEnableVertexAttribArray(posAttribReprojId_);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
  glVertexAttribPointer(posAttribReprojId_, 4, GL_FLOAT, GL_FALSE, 0, 0);

  if (useElements_Indices) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBufferObj_);
    glDrawElements(GL_TRIANGLES, numElements_, GL_UNSIGNED_INT, 0);
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
    glDrawArrays(GL_TRIANGLES, 0, sizeArray_);
  }

  glDisableVertexAttribArray(posAttribReprojId_);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glDisable(GL_CULL_FACE);

}

void ReprojectionProgram::initializeFramebufAndTex(GLuint &imageReprojTex) {
  glGenFramebuffers(1, &framebufferReproj_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebufferReproj_);
  initRedTex(imageReprojTex);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, imageReprojTex, 0);
  GLuint tmpdeptht;
  initDepthTex(tmpdeptht);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tmpdeptht, 0);
  checkFrameBuffer("ReprojectionShaderProgram::initializeFramebufAndTex");
  imageReprojTex_ = imageReprojTex;

}

void ReprojectionProgram::init() {
  shaderManager_.init();
  shaderManager_.addShader(GL_VERTEX_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/reprojection_vertex_shader.glsl");
  shaderManager_.addShader(GL_GEOMETRY_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/reprojection_geometry_shader.glsl");
  shaderManager_.addShader(GL_FRAGMENT_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/reprojection_fragment_shader.glsl");
  shaderManager_.finalize();
}

void ReprojectionProgram::createAttributes() {
  posAttribReprojId_ = shaderManager_.getAttribLocation("position");
}

void ReprojectionProgram::createUniforms() {
  mvp1IDReproj_ = shaderManager_.getUniformLocation("MVP1");
  mvp2IDReproj_ = shaderManager_.getUniformLocation("MVP2");
  image2TexId_ = shaderManager_.getUniformLocation("image2");
  camCenter1ID_ = shaderManager_.getUniformLocation("c1");
  camCenter2ID_ = shaderManager_.getUniformLocation("c2");
  lodId_ = shaderManager_.getUniformLocation("LOD");

  shadowMap1IdReproj_ = shaderManager_.getUniformLocation("shadowMap1");
  shadowMap2IdReproj_ = shaderManager_.getUniformLocation("shadowMap2");
}
}

