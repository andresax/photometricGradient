/*
 * ShaderProgram.h
 *
 *  Created on: 02/apr/2015
 *      Author: andrea
 */

#ifndef SWEEPMESH_SHADERPROGRAM_H_
#define SWEEPMESH_SHADERPROGRAM_H_

#include "ShaderManager.h"
#include <opencv2/core/core.hpp>

class ShaderProgram {
public:
  ShaderProgram(int imageWidth, int imageHeight);
  virtual ~ShaderProgram();
  void initializeProgram();
  virtual void initTex(){};
  virtual void populateTex(const cv::Mat &image1){};
  virtual void populateTex(const cv::Mat &image1, const cv::Mat &image2){};
  virtual void compute(bool renderFrameBuf = false){};
  virtual void createTransformFeedback(int length){};

  void setUseElementsIndices(bool useElementsIndices) {
    useElements_Indices = useElementsIndices;
  }

  void setArrayBufferObj(GLint arrayBufferObj) {
    arrayBufferObj_ = arrayBufferObj;
  }

  void setArrayBufferObj(GLint arrayBufferObj, int sizeArray) {
    arrayBufferObj_ = arrayBufferObj;
    sizeArray_ = sizeArray;
  }

  void setElementsBufferObj(GLint elementsBufferObj) {
    elementsBufferObj_ = elementsBufferObj;
  }

  void setElementsBufferObj(GLint elementsBufferObj, int numElements) {
    elementsBufferObj_ = elementsBufferObj;
    numElements_ = numElements;
  }

  void setNumElements(int numElements) {
    numElements_ = numElements;
  }

  void setSizeArray(int sizeArray) {
    sizeArray_ = sizeArray;
  }

  bool isUseElementsIndices() const {
    return useElements_Indices;
  }

private:
  virtual void init()= 0;

  virtual void createAttributes() = 0;
  virtual void createUniforms() = 0;


protected:
  void defaultTextureParameters();
  void checkFrameBuffer(std::string name);
  void initDepthTex(GLuint& depthTextureId);
  void initRedTex(GLuint& textureId);
  int imageWidth_, imageHeight_;
  bool useElements_Indices;
  GLint arrayBufferObj_, elementsBufferObj_;
  int sizeArray_, numElements_;
  ShaderManager shaderManager_;

};

#endif /* SWEEPMESH_SHADERPROGRAM_H_ */
