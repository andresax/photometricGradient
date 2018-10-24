#include <TextureManager.h>
#include <iostream>

TextureManager::TextureManager(GLenum textureTarget, const std::string& filePath) {
  textureTarget_ = textureTarget;
  textureObj_ = -1;
  filePath_ = filePath;
  image_ = NULL;
}

TextureManager::~TextureManager() {
  glDeleteTextures(1, &textureObj_);
}

bool TextureManager::load() {
  return load(GL_RGB,GL_RGB,GL_UNSIGNED_BYTE);
}

bool TextureManager::load(GLint internalFormat, GLenum format, GLenum type) {
  int widthIM, heightIM;
    unsigned char* image = SOIL_load_image(filePath_.c_str(), &widthIM, &heightIM, 0, SOIL_LOAD_RGB);

    glGenTextures(1, &textureObj_);
    glBindTexture(textureTarget_, textureObj_);
    glTexImage2D(textureTarget_, 0, internalFormat, widthIM, heightIM, 0, format, type, image);

    glTexParameterf(textureTarget_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(textureTarget_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SOIL_free_image_data(image);
    return true;
}

void TextureManager::bind(GLenum textureUnit) {
  glActiveTexture(textureUnit);
  glBindTexture(textureTarget_, textureObj_);
}


void TextureManager::defaultTextureParameters() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

}
void TextureManager::framebufTextureParameters() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

}
