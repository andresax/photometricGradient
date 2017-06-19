#include <GradientFlowProgram.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>


#ifndef BASE_PATH_SHADERS
#define BASE_PATH_SHADERS  "photometricGradient/"
#endif

namespace photometricGradient{
GradientFlowProgram::GradientFlowProgram(int imageWidth, int imageHeight):
ShaderProgram(imageWidth, imageHeight)  {
  lod_=0.0;
}

GradientFlowProgram::~GradientFlowProgram() {
  // TODO Auto-generated destructor stub
}

void GradientFlowProgram::populateTex(const cv::Mat &image1, const cv::Mat &image2) {

  /// Generate grad_x and grad_y
  cv::Mat imageGray, image1Gray, gradXGray, gradYGray;
  cv::cvtColor(image2, imageGray, CV_RGB2GRAY);
  /// Gradient X
  cv::Scharr(imageGray, gradXGray, CV_32F, 1, 0);
  /// Gradient Y
  cv::Scharr(imageGray, gradYGray, CV_32F, 0, 1);

  cv::cvtColor(image1, image1Gray, CV_RGB2GRAY);
  //image1 texture
  glBindTexture(GL_TEXTURE_2D, imageTex_);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imageWidth_, imageHeight_, 0, GL_RED, GL_UNSIGNED_BYTE, image1Gray.data);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RED, imageWidth_, imageHeight_, GL_RED, GL_UNSIGNED_BYTE, image1Gray.data);

  glBindTexture(GL_TEXTURE_2D, textureObjX_);
 // glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, imageWidth_, imageHeight_, 0, GL_RED, GL_FLOAT, gradXGray.data);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_R32F, imageWidth_, imageHeight_, GL_RED, GL_FLOAT, gradXGray.data);

  glBindTexture(GL_TEXTURE_2D, textureObjY_);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, imageWidth_, imageHeight_, 0, GL_RED, GL_FLOAT, gradYGray.data);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_R32F, imageWidth_, imageHeight_, GL_RED, GL_FLOAT, gradYGray.data);

  imageGray.release();
  image1Gray.release();
  gradXGray.release();
  gradYGray.release();
}

void GradientFlowProgram::initializeFramebufAndTex(GLuint& gradTex) {
  glGenFramebuffers(1, &framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

  glGenTextures(1, &gradTex);
  glBindTexture(GL_TEXTURE_2D, gradTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, imageWidth_, imageHeight_, 0, GL_RGB, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gradTex, 0);

  gradTex_ = gradTex;

  GLuint tmpdeptht;
  glGenTextures(1, &tmpdeptht);
  glBindTexture(GL_TEXTURE_2D, tmpdeptht);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, imageWidth_, imageHeight_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tmpdeptht, 0);

  checkFrameBuffer("GradientFlowProgram::initializeFramebufAndTex");

  glGenTextures(1, &imageTex_);
  glBindTexture(GL_TEXTURE_2D, imageTex_);
  defaultTextureParameters();

  glGenTextures(1, &textureObjX_);
  glBindTexture(GL_TEXTURE_2D, textureObjX_);
  defaultTextureParameters();

  glGenTextures(1, &textureObjY_);
  glBindTexture(GL_TEXTURE_2D, textureObjY_);
  defaultTextureParameters();
}

void GradientFlowProgram::compute(bool renderFrameBuf) {
  if (renderFrameBuf){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glViewport(0, 0, imageWidth_, imageHeight_);
  }else{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, imageWidth_, imageHeight_);
  }

  GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, attachments);

  glViewport(0, 0, imageWidth_, imageHeight_);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  shaderManager_.enable();


  glUniform3fv(cam1PositionId_, 1, &t_[0]);
  glUniform3fv(cam2PositionId_, 1, &t2_[0]);
  glUniform1f(lodId_, lod_);
  //std::cout<<""<<t.x<<" , "<<t.y<<",  "<<t.z<<""<<std::endl;
  glUniformMatrix4fv(mvp1ID_, 1, GL_FALSE, &mvp1_[0][0]);
  glUniformMatrix4fv(mvp2ID_, 1, GL_FALSE, &mvp2_[0][0]);
  glUniformMatrix4fv(mvp1OrigID_, 1, GL_FALSE, &mvp1Orig_[0][0]);
  glUniformMatrix4fv(mvp2OrigID_, 1, GL_FALSE, &mvp2Orig_[0][0]);
  glUniformMatrix4fv(pointToCamID_, 1, GL_FALSE, &cam1toPoint[0][0]);

  glActiveTexture(GL_TEXTURE11);
  glBindTexture(GL_TEXTURE_2D, depthTexture_);
  glUniform1i(shadowMapId_, 11);

  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D, depthTexture2_);
  glUniform1i(shadowMapId2_, 8);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, imageTex_);
  glUniform1i(image1Id_, 1);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, textureObjX_);
  glUniform1i(gradimage2xId_, 2);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, textureObjY_);
  glUniform1i(gradimage2yId_, 3);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, simGradTex_);
  glUniform1i(gradSimilarityImgId_, 4);

  glEnableVertexAttribArray(posAttribId_);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
  glVertexAttribPointer(posAttribId_, 4, GL_FLOAT, GL_FALSE, 0, 0);

  if (useElements_Indices) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBufferObj_);
    glDrawElements(GL_TRIANGLES, numElements_, GL_UNSIGNED_INT, 0);

  } else {
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
    glDrawArrays(GL_TRIANGLES, 0, sizeArray_);

  }

  glDisableVertexAttribArray(posAttribId_);
}

void GradientFlowProgram::init() {
  shaderManager_.init();
    shaderManager_.addShader(GL_VERTEX_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/grad_flow_depth_vertex_shader.glsl");
    shaderManager_.addShader(GL_GEOMETRY_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/grad_flow_depth_geometry_shader.glsl");
    shaderManager_.addShader(GL_FRAGMENT_SHADER, std::string(BASE_PATH_SHADERS) + "shaders/grad_flow_depth_fragment_shader.glsl");

    shaderManager_.finalize();
}

void GradientFlowProgram::createAttributes() {
  posAttribId_ = shaderManager_.getAttribLocation("position");
}

void GradientFlowProgram::createUniforms() {
  cam1PositionId_ = shaderManager_.getUniformLocation("camPosition");
  cam2PositionId_ = shaderManager_.getUniformLocation("cam2Position");
  mvp1ID_ = shaderManager_.getUniformLocation("MVPcam1");
  mvp2ID_ = shaderManager_.getUniformLocation("MVPcam2");
  mvp1OrigID_ = shaderManager_.getUniformLocation("MVPcam1Orig");
  mvp2OrigID_ = shaderManager_.getUniformLocation("MVPcam2Orig");
  pointToCamID_ = shaderManager_.getUniformLocation("pointTocamMat2");

  mvpCameraMoving_ = shaderManager_.getUniformLocation("MVP2");
  image1Id_ = shaderManager_.getUniformLocation("image1");
  shadowMapId_ = shaderManager_.getUniformLocation("shadowMap");
  shadowMapId2_ = shaderManager_.getUniformLocation("shadowMap2");

  gradimage2yId_ = shaderManager_.getUniformLocation("gradimage2y");
  gradimage2xId_ = shaderManager_.getUniformLocation("gradimage2x");
  gradSimilarityImgId_ = shaderManager_.getUniformLocation("gradSimilarityImg");
  lod_ = shaderManager_.getUniformLocation("LOD");
}
}
