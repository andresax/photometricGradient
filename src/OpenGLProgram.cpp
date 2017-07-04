#include <OpenGLProgram.h>

#include <cstdio>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

OpenGLProgram::OpenGLProgram() {
  window_ = nullptr;
  imageWidth_ = -1;
  imageHeight_ = -1;
  vertexArrayObj_ = -1;
  eboSimGrad_ = -1;
  vboSimGrad_ = -1;
}

OpenGLProgram::~OpenGLProgram() {

}

void OpenGLProgram::CaptureViewPort(cv::Mat &capture, GLenum format, GLuint ch) {

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  GLubyte * bits; //RGB bits
  GLint viewport[4]; //current viewport

  int w = imageWidth_;
  int h = imageHeight_;

  bits = new GLubyte[w * ch * h];

  //read pixel from frame buffer
  glPixelStorei(GL_PACK_ALIGNMENT, 1); //or glPixelStorei(GL_PACK_ALIGNMENT,4);
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  glPixelStorei(GL_PACK_SKIP_ROWS, 0);
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
  glReadPixels(0, 0, w, h, format, GL_UNSIGNED_BYTE, bits);

  cv::Mat tem = cv::Mat(h,w,CV_8UC3);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      for (int curCh = 0; curCh < ch; ++curCh) {
        tem.at<cv::Vec3b>(i, j)[2-curCh] = (unsigned char) (bits[(h - i - 1) * 3 * w + j * ch + curCh]);
      }
    }
  }

  capture =tem;

  cv::imwrite("result.png", capture);
  tem.release();
  delete[] bits;
}

void OpenGLProgram::CaptureViewPortFloat(cv::Mat& capture, GLenum format, GLuint ch, GLuint framebuf) {

  glBindFramebuffer(GL_FRAMEBUFFER, framebuf);
  GLfloat * bits; //RGB bits
  GLint viewport[4]; //current viewport

  int w = imageWidth_;
  int h = imageHeight_;

  bits = new GLfloat[w * ch * h];

  //read pixel from frame buffer
  glFinish(); //finish all commands of OpenGL
  glPixelStorei(GL_PACK_ALIGNMENT, 1); //or glPixelStorei(GL_PACK_ALIGNMENT,4);
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  glPixelStorei(GL_PACK_SKIP_ROWS, 0);
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
  glReadPixels(0, 0, w, h, format, GL_FLOAT, bits);

  cv::Mat tem = cv::Mat(h,w,CV_8UC3);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      for (int curCh = 0; curCh < ch; ++curCh) {
        tem.at<cv::Vec3b>(i, j)[2-curCh] = 255*(float) (bits[(h - i - 1) * ch * w + j * ch + curCh]);
      }
    }
  }
//  capture = capImg;

  capture = tem;
  cv::imwrite("result.png", capture);
  delete[] bits;
}

void error_callback(int error, const char* description)
{
    puts(description);
}

bool OpenGLProgram::initGlwf() {
  // Initialize GLFW
  int errcode = glfwInit();
  if (!errcode) {
    glfwSetErrorCallback(error_callback);
    fprintf(stderr, "Failed to initialize GLFW\n");
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  window_ = glfwCreateWindow(imageWidth_, imageHeight_, "mum...mumble", nullptr, nullptr);

  if (window_ == NULL) {
    fprintf(stderr, "Failed to open GLFW window.\n");
    glfwTerminate();
    return false;
  }
  return true;
}

void OpenGLProgram::SwapBuffers() {

  glfwSwapBuffers(window_);
  glfwPollEvents();
}

void OpenGLProgram::init() {
  glewExperimental=GL_TRUE;
  initGlwf();
  glfwMakeContextCurrent(window_);
   if (glewInit() != GLEW_OK) {
    std::cerr << "Glew failed to init " << std::endl;
  }else{

    std::cout << "Glew init " << std::endl;
  }
  glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
  glClearColor(0.0f, 0.0f, 0.00f, 0.0f);

  glGenVertexArrays(1, &vertexArrayObj_);
  glBindVertexArray(vertexArrayObj_);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

}

void OpenGLProgram::createImageVerticesBuffer() {
// Create a Vertex Buffer Object and copy the vertex data to it
  glGenBuffers(1, &vboSimGrad_);

  GLfloat vertices[] = {
//  Position   Texcoords
      -1.0f, 1.0f, 0.0f, 0.0f, // Top-left
      1.0f, 1.0f, 1.0f, 0.0f, // Top-right
      1.0f, -1.0f, 1.0f, 1.0f, // Bottom-right
      -1.0f, -1.0f, 0.0f, 1.0f  // Bottom-left
      };

  glBindBuffer(GL_ARRAY_BUFFER, vboSimGrad_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// Create an element array
  glGenBuffers(1, &eboSimGrad_);

  GLuint elements[] = { 0, 1, 2, 2, 3, 0 };

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboSimGrad_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

}

void OpenGLProgram::defaultTextureParameters() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

}
void OpenGLProgram::framebufTextureParameters() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

}

void OpenGLProgram::checkLastError() {
  GLenum res = glGetError();
  switch (res) {
  case GL_NO_ERROR:
    std::cerr << "GL_NO_ERROR" << std::endl;
    break;
  case GL_INVALID_ENUM:
    std::cerr << "GL_INVALID_ENUM" << std::endl;
    break;
  case GL_INVALID_VALUE:
    std::cerr << "GL_INVALID_VALUE" << std::endl;
    break;
  case GL_INVALID_OPERATION:
    std::cerr << "GL_INVALID_OPERATION" << std::endl;
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
    break;
  case GL_OUT_OF_MEMORY:
    std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
    break;
  case GL_STACK_UNDERFLOW:
    std::cerr << "GL_STACK_UNDERFLOW" << std::endl;
    break;
  case GL_STACK_OVERFLOW:
    std::cerr << "GL_STACK_OVERFLOW" << std::endl;
    break;
  default:
    break;
  }
}
