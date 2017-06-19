#include <DepthMapProgram.h>

#ifndef BASE_PATH_SHADERS
#define BASE_PATH_SHADERS  "photometricGradient/"
#endif

namespace photometricGradient{
DepthMapProgram::DepthMapProgram(int imageWidth, int imageHeight) :
    ShaderProgram(imageWidth, imageHeight) {
  posAttribDepthId_ = -1;
  mvpId_  = -1;
}

DepthMapProgram::~DepthMapProgram() {
}


void DepthMapProgram::initializeFramebufAndTex(GLuint& framebufferDepth, GLuint& depthTextureId) {
  glGenFramebuffers(1, &framebufferDepth);
  glBindFramebuffer(GL_FRAMEBUFFER, framebufferDepth);
  initDepthTex(depthTextureId);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureId, 0);
  glDrawBuffer(GL_NONE);
  checkFrameBuffer("DepthShaderProgram::initializeFramebufAndTex");
}

void DepthMapProgram::computeDepthMap(const GLuint &framebufferDepth, const glm::mat4 &mvp) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebufferDepth);
  glViewport(0, 0, imageWidth_, imageHeight_);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shaderManager_.enable();

  glUniformMatrix4fv(mvpId_, 1, GL_FALSE, &mvp[0][0]);

  glEnableVertexAttribArray(posAttribDepthId_);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
  glVertexAttribPointer(posAttribDepthId_, 4, GL_FLOAT, GL_FALSE, 0, 0);

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

void DepthMapProgram::init() {
  shaderManager_.init();
  shaderManager_.addShader(GL_VERTEX_SHADER, std::string(BASE_PATH_SHADERS)+"shaders/depth_vertex_shader.glsl");
  shaderManager_.addShader(GL_FRAGMENT_SHADER, std::string(BASE_PATH_SHADERS)+"shaders/depth_fragment_shader.glsl");
  shaderManager_.finalize();
}

void DepthMapProgram::createAttributes() {
  posAttribDepthId_ = shaderManager_.getAttribLocation("position");
}
void DepthMapProgram::createUniforms() {
  mvpId_ = shaderManager_.getUniformLocation("MVP");
}

}
