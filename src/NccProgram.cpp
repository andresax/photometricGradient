#include <NccProgram.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <GL/glew.h>
#include <highgui.h>
#ifndef BASE_PATH_SHADERS
#define BASE_PATH_SHADERS  "photometricGradient/"
#endif

namespace photometricGradient {
NccProgram::NccProgram(int imageWidth, int imageHeight) :
    ShaderProgram(imageWidth, imageHeight) {
  imWid_ = imHid_ = nccTexId_ = wId_ = -1;
  framebuffer_ = image2ReprojTex_ = imageTex_ = -1;
  nccTexId_ = image2ReprojTexId_ = imageTexId_ = -1;

  posId_ = texCoordId_ = -1;
  window_ = -1;
  varTexId_ = -1;
  meanTexId_ = -1;
  reliabTexId_ = -1;
  lodId_ = lod_ = -1;
}

NccProgram::~NccProgram() {
}
void NccProgram::initTex() {
  glGenTextures(1, &imageTex_);
  glBindTexture(GL_TEXTURE_2D, imageTex_);
  defaultTextureParameters();
}

void NccProgram::populateTex(const cv::Mat& image) {

  cv::Mat image2Gray;
  if (image.channels() > 1) {
    cv::cvtColor(image, image2Gray, CV_RGB2GRAY);
  } else {
    image2Gray = image;
  }
  glBindTexture(GL_TEXTURE_2D, imageTex_);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imageWidth_, imageHeight_, 0, GL_RED, GL_UNSIGNED_BYTE, image2Gray.data);

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RED, imageWidth_, imageHeight_, GL_RED, GL_UNSIGNED_BYTE, image2Gray.data);
  image2Gray.release();
}

void NccProgram::initializeFramebufAndTex(GLuint &nccTexId, GLuint &varTexId, GLuint &meanTexId, GLuint &reliabilityId) {
  glGenFramebuffers(1, &framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  glGenTextures(1, &nccTexId);
  glBindTexture(GL_TEXTURE_2D, nccTexId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, imageWidth_, imageHeight_, 0, GL_RED, GL_FLOAT, nullptr);
  defaultTextureParameters();
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, nccTexId, 0);

  glGenTextures(1, &varTexId);
  glBindTexture(GL_TEXTURE_2D, varTexId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, imageWidth_, imageHeight_, 0, GL_RGB, GL_FLOAT, nullptr);
  defaultTextureParameters();
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, varTexId, 0);
  glGenTextures(1, &meanTexId);
  glBindTexture(GL_TEXTURE_2D, meanTexId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, imageWidth_, imageHeight_, 0, GL_RG, GL_FLOAT, nullptr);
  defaultTextureParameters();
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, meanTexId, 0);

  glGenTextures(1, &reliabilityId);
  glBindTexture(GL_TEXTURE_2D, reliabilityId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, imageWidth_, imageHeight_, 0, GL_RED, GL_FLOAT, nullptr);
  defaultTextureParameters();
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, reliabilityId, 0);

  nccTexId_ = nccTexId;
  varTexId_ = varTexId;
  meanTexId_ = meanTexId;
  reliabTexId_ = reliabilityId;

  checkFrameBuffer("NccProgram::initializeFramebufAndTex");

}

void NccProgram::compute(bool renderFrameBuf) {
  if (renderFrameBuf)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  else
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  GLuint attachments2[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  glDrawBuffers(4, attachments2);

  glViewport(0, 0, imageWidth_, imageHeight_);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shaderManager_.enable();

  glUniform1f(imWid_, static_cast<float>(imageWidth_));
  glUniform1f(imHid_, static_cast<float>(imageHeight_));
  glUniform1f(wId_, static_cast<float>(window_));
  glUniform1f(lodId_, lod_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, imageTex_);
  glUniform1i(imageTexId_, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, image2ReprojTex_);
  glUniform1i(image2ReprojTexId_, 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, depthXYZ_);
  glUniform1i(depthXYZId_, 2);


  glActiveTexture(GL_TEXTURE11);
  glBindTexture(GL_TEXTURE_2D, depthTexture_);
  glUniform1i(shadowMapId_, 11);

  glEnableVertexAttribArray(posId_);
  glEnableVertexAttribArray(texCoordId_);

  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
  glVertexAttribPointer(posId_, 2, GL_FLOAT, GL_FALSE, sizeArray_ * sizeof(GLfloat), 0);
  glVertexAttribPointer(texCoordId_, 2, GL_FLOAT, GL_FALSE, sizeArray_ * sizeof(GLfloat), (const GLvoid*) (2 * sizeof(GLfloat)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBufferObj_);
  glDrawElements(GL_TRIANGLES, numElements_, GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(posId_);
  glDisableVertexAttribArray(texCoordId_);

}

void NccProgram::init() {
  shaderManager_.init();
  shaderManager_.addShader(GL_VERTEX_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/ncc_vertex_shader.glsl");
  shaderManager_.addShader(GL_FRAGMENT_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/ncc_fragment_shader.glsl");
  shaderManager_.finalize();
}

void NccProgram::createAttributes() {
  posId_ = shaderManager_.getAttribLocation("position");
  texCoordId_ = shaderManager_.getAttribLocation("texcoord");
}

void NccProgram::createUniforms() {
  imageTexId_ = shaderManager_.getUniformLocation("image1");
  image2ReprojTexId_ = shaderManager_.getUniformLocation("image2Repr");
  depthXYZId_ = shaderManager_.getUniformLocation("depthXYZ");
  imWid_ = shaderManager_.getUniformLocation("imW");
  imHid_ = shaderManager_.getUniformLocation("imH");
  wId_ = shaderManager_.getUniformLocation("window");
  lodId_ = shaderManager_.getUniformLocation("LOD");
  shadowMapId_ = shaderManager_.getUniformLocation("shadowMap");
}
}

