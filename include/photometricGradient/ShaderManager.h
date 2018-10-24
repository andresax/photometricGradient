#ifndef SHADERMANAGER_H_
#define SHADERMANAGER_H_

#include <vector>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

class ShaderManager {

  public:
    ShaderManager();

    virtual ~ShaderManager();

    bool init();

    bool addShader(GLenum shaderType, std::string path);

    bool finalize();

    bool enable();

    GLint getAttribLocation(std::string attributeName);
    GLint getUniformLocation(std::string uniformName);
    void addFeedbackTransform(std::string nameVar);
    void printActiveUniforms();
  private:
    typedef std::vector<GLuint> ShaderObjList_;
    ShaderObjList_ shaderObjList_;
    GLuint shaderProg_;

    std::string getShaderTypeString(GLenum shaderType);

    GLuint loadShaderFromFile(std::string path, GLenum shaderType);
};

#endif /* SHADERMANAGER_H_ */
