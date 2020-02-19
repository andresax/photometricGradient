#include <ShaderProgram.h>

ShaderProgram::ShaderProgram(int imageWidth, int imageHeight) {
  imageHeight_ = imageHeight;
  imageWidth_ = imageWidth;
  useElements_Indices = true;
  arrayBufferObj_ = -1;
  elementsBufferObj_ = -1;
  sizeArray_ = 0;
  numElements_ = 0;
}

ShaderProgram::~ShaderProgram() {
}

void ShaderProgram::initializeProgram() {
  init();

  createAttributes();
  createUniforms();
}

void ShaderProgram::initDepthTex(GLuint& depthTextureId) {

  // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
  glGenTextures(1, &depthTextureId);
  glBindTexture(GL_TEXTURE_2D, depthTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, imageWidth_, imageHeight_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);


}


void ShaderProgram::initRedTex(GLuint& textureId) {
  glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth_, imageHeight_, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    defaultTextureParameters();

}



void ShaderProgram::defaultTextureParameters() {
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void ShaderProgram::checkFrameBuffer(std::string name) {

  GLenum returnStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (returnStatus != GL_FRAMEBUFFER_COMPLETE) {
      std::cerr << name << std::endl;
  std::cerr << "glCheckFramebufferStatus returns";

  switch (returnStatus) {
  case GL_FRAMEBUFFER_UNDEFINED:
    std::cerr << "GL_FRAMEBUFFER_UNDEFINED" << std::endl;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl;
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED:
    std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
    std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << std::endl;
    break;
  default:
    break;
  }
  }

}
