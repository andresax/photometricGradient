#include <GradientCollectorProgram.h>
#include <Logger.h>


namespace photometricGradient{
GradientCollectorProgram::GradientCollectorProgram(int imageWidth, int imageHeight) :
    ShaderProgram(imageWidth, imageHeight) {
      lod_=0.0;
}

GradientCollectorProgram::~GradientCollectorProgram() {
}

void GradientCollectorProgram::compute(bool show) {
  utilities::Logger log;

  log.disable();
  log.startEvent();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  shaderManager_.enable();

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, gradientValuesTexture_);
  glUniform1i(gradId_, 5);

  glUniformMatrix4fv(mvp1CollID_, 1, GL_FALSE, &mvp1[0][0]);
  glUniformMatrix4fv(mvp2CollID_, 1, GL_FALSE, &mvp2[0][0]);
  glUniform1f(imWCollectorid_, static_cast<float>(imageWidth_));
  glUniform1f(imHCollectorid_, static_cast<float>(imageHeight_));
    glUniform1f(lodId_, lod_);

  glActiveTexture(GL_TEXTURE11);
  glBindTexture(GL_TEXTURE_2D, depthTexture_);
  glUniform1i(shadowMapIdColl_, 11);

  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D, depthTexture2_);
  glUniform1i(shadowMapId2Coll_, 8);

  glEnableVertexAttribArray(posAttribCollId_);
  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
  glVertexAttribPointer(posAttribCollId_, 3, GL_FLOAT, GL_FALSE, 0, 0);

  if (!show)
    glEnable(GL_RASTERIZER_DISCARD);

  glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbackBuffer_);
  glBeginTransformFeedback(GL_TRIANGLES);

  log.startEvent();
  if (useElements_Indices) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBufferObj_);
    glDrawElements(GL_TRIANGLES, numElements_, GL_UNSIGNED_INT, 0);

  } else {
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferObj_);
    glDrawArrays(GL_TRIANGLES, 0, sizeArray_);

  }
  log.endEventAndPrint("Grad Collect::glDrawElements ", false);
  //sleep(1.0);
  log.startEvent();
  glEndTransformFeedback();
  glDisableVertexAttribArray(posAttribCollId_);
  log.endEventAndPrint("Grad Collect::glEndTransformFeedback ", false);

  log.startEvent();
  glFinish();
  log.endEventAndPrint("Grad Collect::glFinish ", false);

  if (!show)
    glDisable(GL_RASTERIZER_DISCARD);

  log.endEventAndPrint("Grad Collect::compute ", false);
  log.startEvent();
  if (useElements_Indices) {
    feedbackTr.resize(numElements_, glm::vec3(0.0));
    feedbackLength_ = numElements_;

  } else {
    feedbackTr.resize(sizeArray_, glm::vec3(0.0));
    feedbackLength_ = sizeArray_;

  }

  glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbackTr.size() * sizeof(glm::vec3), &feedbackTr[0]);

  log.endEventAndPrint("Grad Collect::transffeed ", false);
}

void GradientCollectorProgram::createTransformFeedback(int length) {
  glGenBuffers(1, &feedbackBuffer_);
  glBindBuffer(GL_ARRAY_BUFFER, feedbackBuffer_);
  glBufferData(GL_ARRAY_BUFFER, length * sizeof(glm::vec3), nullptr, GL_DYNAMIC_READ);
}

void GradientCollectorProgram::resetTransformFeedback(int length) {
  glBindBuffer(GL_ARRAY_BUFFER, feedbackBuffer_);
  glBufferData(GL_ARRAY_BUFFER, length * sizeof(glm::vec3), nullptr, GL_DYNAMIC_READ);
}

void GradientCollectorProgram::init() {
  shaderManager_.init();
  shaderManager_.addShader(GL_VERTEX_SHADER, "shaders/grad_coll_vertex_shader.glsl");
  shaderManager_.addShader(GL_GEOMETRY_SHADER, "shaders/grad_coll_geometry_shader.glsl");
  shaderManager_.addFeedbackTransform("sumGradient");
  shaderManager_.addShader(GL_FRAGMENT_SHADER, "shaders/grad_coll_fragment_shader.glsl");
  shaderManager_.finalize();
}

void GradientCollectorProgram::createAttributes() {
  posAttribCollId_ = shaderManager_.getAttribLocation("position");
}

void GradientCollectorProgram::createUniforms() {
  gradId_ = shaderManager_.getUniformLocation("gradMap");
  mvp1CollID_ = shaderManager_.getUniformLocation("MVPcam1");
  mvp2CollID_ = shaderManager_.getUniformLocation("MVPcam2");
  shadowMapIdColl_ = shaderManager_.getUniformLocation("shadowMap");
  shadowMapId2Coll_ = shaderManager_.getUniformLocation("shadowMap2");
  imWCollectorid_ = shaderManager_.getUniformLocation("imW");
  imHCollectorid_ = shaderManager_.getUniformLocation("imH");
  lodId_ = shaderManager_.getUniformLocation("LOD");
}
}