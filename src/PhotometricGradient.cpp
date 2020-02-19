#include <PhotometricGradient.h>
#include <DepthMapProgram.h>
#include <DepthMapXYZProgram.h>
#include <ReprojectionProgram.h>
#include <NccProgram.h>
#include <NccGradientProgram.h>
#include <GradientFlowProgram.h>
#include <GradientCollectorProgram.h>
#include <Logger.h>
#include <unistd.h>
#include <opencv2/highgui.hpp>

namespace photometricGradient{
PhotometricGradient::PhotometricGradient(int imageWidth, int imageHeight, GLFWwindow* window) {
  window_ = window;
  imageWidth_ = imageWidth;
  imageHeight_ = imageHeight;
  window_NCC_ = 4;
  hasWeights=true;
  initShaders();
}

PhotometricGradient::~PhotometricGradient() {

}

void PhotometricGradient::initShaders() {

  //************************depth********************************
  std::cout << "DepthMapProgram init...";
  depthMapProgram_ = new DepthMapProgram(imageWidth_, imageHeight_);
  depthMapProgram_->initializeProgram();
  depthMapProgram_->setUseElementsIndices(false);
  static_cast<DepthMapProgram *>(depthMapProgram_)->initializeFramebufAndTex(framebufferDepth_, depthTexture_);
  static_cast<DepthMapProgram *>(depthMapProgram_)->initializeFramebufAndTex(framebufferDepth2_, depthTexture2_);
  std::cout << "DONE" << std::endl;

  //************************depthXYZ********************************
  std::cout << "DepthMapProgram init...";
  depthMapXYZProgram_ = new DepthMapXYZProgram(imageWidth_, imageHeight_);
  depthMapXYZProgram_->initializeProgram();
  depthMapXYZProgram_->setUseElementsIndices(false);
  static_cast<DepthMapXYZProgram *>(depthMapXYZProgram_)->initializeFramebufAndTex( depthTextureXYZ_);
  std::cout << "DONE" << std::endl;

  //************************reprojection**************************
  std::cout << "ReprojectionProgram init...";
  reprojectionProgram_ = new ReprojectionProgram(imageWidth_, imageHeight_);
  reprojectionProgram_->initializeProgram();
  static_cast<ReprojectionProgram *>(reprojectionProgram_)->initTex();
  reprojectionProgram_->setUseElementsIndices(false);
  static_cast<ReprojectionProgram *>(reprojectionProgram_)->initializeFramebufAndTex(image2ReprojTex_);
  std::cout << "DONE" << std::endl;

  //**********************ncc*************************************
  std::cout << "NccProgram init...";
  nccProgram_ = new NccProgram(imageWidth_, imageHeight_);
  nccProgram_->initializeProgram();
  static_cast<NccProgram*>(nccProgram_)->initTex();
  nccProgram_->setUseElementsIndices(true);
  static_cast<NccProgram*>(nccProgram_)->initializeFramebufAndTex(NCCTex_, varTex_, meanTex_, reliabilityTex_);
  std::cout << "DONE" << std::endl;

  //**********************grad ncc*************************************
  std::cout << "NccGradientProgram init...";
  nccGradProgram_ = new NccGradientProgram(imageWidth_, imageHeight_);
  nccGradProgram_->initializeProgram();
  nccGradProgram_->setUseElementsIndices(true);
  static_cast<NccGradientProgram *>(nccGradProgram_)->initializeFramebufAndTex(simGradTex_);
  std::cout << "DONE" << std::endl;

  //**********************grad flow*************************************
  std::cout << "GradientFlowProgram init...";
  gradFlowProgram_ = new GradientFlowProgram(imageWidth_, imageHeight_);
  gradFlowProgram_->initializeProgram();
  gradFlowProgram_->setUseElementsIndices(false);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->initializeFramebufAndTex(gradientValuesTexture_);
  std::cout << "DONE" << std::endl;

  //**********************grad coll*************************************
  std::cout << "GradientCollectorProgram init...";
  gradCollProgram_ = new GradientCollectorProgram(imageWidth_, imageHeight_);
  gradCollProgram_->initializeProgram();
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->createTransformFeedback(0);
  gradCollProgram_->setUseElementsIndices(false);
  std::cout << "DONE" << std::endl;

}

const std::vector<glm::vec3>& PhotometricGradient::twoImageGradient(const cv::Mat& image1, const cv::Mat& image2, const photometricGradient::CameraType& cam1,
    const photometricGradient::CameraType& cam2, int numActiveVertices, int levelOfDetail,bool useSSD) {
  numActiveVertices_ = numActiveVertices;
  utilities::Logger logger;
  logger.disable();
  glm::mat4 mvp1 = cam1.mvp;
  glm::mat4 mvp2 = cam2.mvp;

  glm::mat4 mvp1Orig = cam1.cameraMatrix;
  glm::mat4 mvp2Orig = cam2.cameraMatrix;
  glm::mat4 cam1toPoint = cam1.mvp;
  glm::vec3 t, t2;
  t = -cam1.translation * glm::transpose(cam1.rotation);
  t2 = -cam2.translation * glm::transpose(cam2.rotation);

  logger.startEvent();

  //******************DEPTH MAP *******************************
  logger.startEvent();
  depthMapProgram_->setArrayBufferObj(vertexBufferObj_, numActiveVertices_);
  static_cast<DepthMapProgram *>(depthMapProgram_)->computeDepthMap(framebufferDepth_, mvp1);
  static_cast<DepthMapProgram *>(depthMapProgram_)->computeDepthMap(framebufferDepth2_, mvp2);
  glFinish();
  logger.endEventAndPrint("Depth ", false);


  //******************DEPTH MAP XYZ*******************************
  logger.startEvent();
  depthMapXYZProgram_->setArrayBufferObj(vertexBufferObj_, numActiveVertices_);
  static_cast<DepthMapXYZProgram *>(depthMapXYZProgram_)->setMvp(mvp1);
  static_cast<DepthMapXYZProgram *>(depthMapXYZProgram_)->setCenter(cam1.center);
  static_cast<DepthMapXYZProgram *>(depthMapXYZProgram_)->setDepthTexture(depthTexture_);
  static_cast<DepthMapXYZProgram *>(depthMapXYZProgram_)->compute(true);
  glFinish();
  logger.endEventAndPrint("DepthXYZ ", false);

  //**********************REPROJ*******************************
  logger.startEvent();
  reprojectionProgram_->setArrayBufferObj(vertexBufferObj_, numActiveVertices_);

  static_cast<ReprojectionProgram *>(reprojectionProgram_)->setDepthTexture(depthTexture_, depthTexture2_);
  static_cast<ReprojectionProgram *>(reprojectionProgram_)->setMvp(mvp1, mvp2);
  static_cast<ReprojectionProgram *>(reprojectionProgram_)->setCamCenter1(cam1.center);
  static_cast<ReprojectionProgram *>(reprojectionProgram_)->setCamCenter2(cam2.center);
  static_cast<ReprojectionProgram *>(reprojectionProgram_)->setLod(levelOfDetail);
  reprojectionProgram_->populateTex(image2);
  reprojectionProgram_->compute(true);
  glFinish();
  logger.endEventAndPrint("Reproj ", false);

  //************************NCC***********************************
  logger.startEvent();
  nccProgram_->setArrayBufferObj(vboSimGrad_, 4);
  nccProgram_->setElementsBufferObj(eboSimGrad_, 6);
  static_cast<NccProgram *>(nccProgram_)->setImage2ReprojTex(image2ReprojTex_);
  static_cast<NccProgram *>(nccProgram_)->setDepthXYZ(depthTextureXYZ_);
  nccProgram_->populateTex(image1);
  static_cast<NccProgram *>(nccProgram_)->setWindow(window_NCC_);
  static_cast<NccProgram *>(nccProgram_)->setLod(levelOfDetail);
  nccProgram_->compute(true);
  glFinish();
  logger.endEventAndPrint("Ncc ", true);



  //*******************GRAD NCC***********************************
  if(!useSSD){
    logger.startEvent();
    nccGradProgram_->setArrayBufferObj(vboSimGrad_, 4);
    nccGradProgram_->setElementsBufferObj(eboSimGrad_, 6);
    static_cast<NccGradientProgram *>(nccGradProgram_)->setImage2ReprojTex(image2ReprojTex_);
    static_cast<NccGradientProgram *>(nccGradProgram_)->setNccTexId(NCCTex_);
    static_cast<NccGradientProgram *>(nccGradProgram_)->setVarTexId(varTex_);
    static_cast<NccGradientProgram *>(nccGradProgram_)->setMeanTexId(meanTex_);
    static_cast<NccGradientProgram *>(nccGradProgram_)->setReliabTexId(reliabilityTex_);
    static_cast<NccGradientProgram *>(nccGradProgram_)->setDepthXYZ(depthTextureXYZ_);
    static_cast<NccGradientProgram *>(nccGradProgram_)->setWindow(window_NCC_);
    static_cast<NccGradientProgram *>(nccGradProgram_)->setLod(levelOfDetail);
    nccGradProgram_->populateTex(image1);
    nccGradProgram_->compute(true);
    glFinish();
    logger.endEventAndPrint("Grad Ncc ", false);
  }


  //*******************GRAD Flow***********************************
  logger.startEvent();
  gradFlowProgram_->setArrayBufferObj(vertexBufferObj_, numActiveVertices_);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setCam1toPoint(cam1toPoint);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setDepthTexture(depthTexture_);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setDepthTexture2(depthTexture2_);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setMvp1(mvp1);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setMvp1Orig(mvp1Orig);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setMvp2(mvp2);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setMvp2Orig(mvp2Orig);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setHasW(hasWeights);
  if(!useSSD){
    static_cast<GradientFlowProgram *>(gradFlowProgram_)->setSimGradTex(simGradTex_);
  }else{
    static_cast<GradientFlowProgram *>(gradFlowProgram_)->setSimGradTex(reliabilityTex_);
  }
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setT(t);
  static_cast<GradientFlowProgram *>(gradFlowProgram_)->setT2(t2);

  gradFlowProgram_->populateTex(image1, image2);

  gradFlowProgram_->compute(true);
  glFinish();
  logger.endEventAndPrint("Grad Flow ", false);

  //*******************GRAD COLL***********************************
  logger.startEvent();
  gradCollProgram_->setArrayBufferObj(vertexBufferObj_, numActiveVertices_);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->resetTransformFeedback(numActiveVertices_);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->setMvp1(mvp1);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->setMvp2(mvp2);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->setCamCenter1(cam1.center);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->setCamCenter2(cam2.center);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->setFeedbackLength(numActiveVertices_);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->setDepthTexture(depthTexture_);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->setDepthTexture2(depthTexture2_);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->setGradientValuesTexture(gradientValuesTexture_);
  static_cast<GradientCollectorProgram *>(gradCollProgram_)->compute(false);
  logger.endEventAndPrint("Grad Collect ", false);

  //*/
  SwapBuffers();
//  if(useSSD)
// sleep(20.0);
 //exit(0);
  //cv::imshow("mopinp",image1);cv::waitKey();

  logger.endEventAndPrint("\nTotal twoimages", true);

  return static_cast<GradientCollectorProgram *>(gradCollProgram_)->getFeedbackTr();
}
}
