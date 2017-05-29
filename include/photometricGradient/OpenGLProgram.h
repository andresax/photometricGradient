#ifndef OPENGLPROGRAM_H_
#define OPENGLPROGRAM_H_

#include <GL/glew.h>
#include <opencv2/core/core.hpp>

struct GLFWwindow;

class OpenGLProgram {
public:
  OpenGLProgram();
  virtual ~OpenGLProgram();

  void run();

  void setImageHeight(int imageHeight) {
    imageHeight_ = imageHeight;
  }

  void setImageWidth(int imageWidth) {
    imageWidth_ = imageWidth;
  }

  void setEboSimGrad(GLuint eboSimGrad) {
    eboSimGrad_ = eboSimGrad;
  }

  void setVboSimGrad(GLuint vboSimGrad) {
    vboSimGrad_ = vboSimGrad;
  }

  GLFWwindow* getWindow() const {
    return window_;
  }

  void setWindow(GLFWwindow* window) {
    window_ = window;
  }

protected:

  void SwapBuffers();
  void CaptureViewPort(cv::Mat &capture, GLenum format = GL_BGR_EXT,  GLuint ch=3);
  void CaptureViewPortFloat(cv::Mat &capture, GLenum format = GL_BGR_EXT, GLuint ch=3);
  bool initGlwf();
  void init();
  void checkLastError();


  void createImageVerticesBuffer();
  void defaultTextureParameters();
  void framebufTextureParameters();

  GLFWwindow* window_;
  int imageWidth_, imageHeight_;
  GLuint vertexArrayObj_;
  GLuint vboSimGrad_, eboSimGrad_;
};


#endif /* OPENGLWRAPPER_OPENGLPROGRAM_H_ */
