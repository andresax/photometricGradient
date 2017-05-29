#include <fstream>

#include <ShaderManager.h>

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

ShaderManager::ShaderManager() {
  shaderProg_ = 0;

}

ShaderManager::~ShaderManager() {
  // Delete the intermediate shader objects that have been added to the program
  // The list will only contain something if shaders were compiled but the object itself
  // was destroyed prior to linking.
  for (ShaderObjList_::iterator it = shaderObjList_.begin(); it != shaderObjList_.end(); it++) {
    glDeleteShader(*it);
  }

  if (shaderProg_ != 0) {
    glDeleteProgram(shaderProg_);
    shaderProg_ = 0;
  }
}

bool ShaderManager::init() {
  shaderProg_ = glCreateProgram();

  if (shaderProg_ == 0) {
    fprintf(stderr, "Error creating shader program\n");
    return false;
  }
  return true;
}

bool ShaderManager::addShader(GLenum shaderType, std::string path) {
  GLuint shaderObj = loadShaderFromFile(path, shaderType);

  if (shaderObj == 0) {
    fprintf(stderr, "Error creating shader type %s\n", getShaderTypeString(shaderType).c_str());
    return false;
  }

  shaderObjList_.push_back(shaderObj);

  glAttachShader(shaderProg_, shaderObj);

  return true;
}

// After all the shaders have been added to the program call this function
// to link and validate the program.
bool ShaderManager::finalize() {
  GLint Success = 0;
  GLchar ErrorLog[1024] = { 0 };

  glLinkProgram(shaderProg_);

  glGetProgramiv(shaderProg_, GL_LINK_STATUS, &Success);
  if (Success == 0) {
    glGetProgramInfoLog(shaderProg_, sizeof(ErrorLog), NULL, ErrorLog);
    fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
    return false;
  }

  glValidateProgram(shaderProg_);
  glGetProgramiv(shaderProg_, GL_VALIDATE_STATUS, &Success);
  if (!Success) {
    glGetProgramInfoLog(shaderProg_, sizeof(ErrorLog), NULL, ErrorLog);
    fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
    return false;
  }

  // Delete the intermediate shader objects that have been added to the program
  for (ShaderObjList_::iterator it = shaderObjList_.begin(); it != shaderObjList_.end(); it++) {
    glDeleteShader(*it);
  }

  shaderObjList_.clear();

  return true;
}

bool ShaderManager::enable() {
  glUseProgram(shaderProg_);
  return true;
}
GLint ShaderManager::getAttribLocation(std::string attributeName) {
  GLint attrID = glGetAttribLocation(shaderProg_, attributeName.c_str());
  return attrID;
}
GLint ShaderManager::getUniformLocation(std::string uniformName) {
  GLint uniID = glGetUniformLocation(shaderProg_, uniformName.c_str());

  if (uniID == -1) {
    std::cerr << "Error: " << uniformName << " not found in the shaders" << std::endl;
  }

  //assert(uniID != -1);
  return uniID;
}

std::string ShaderManager::getShaderTypeString(GLenum shaderType) {
  std::string value;

  switch (shaderType) {
    case GL_VERTEX_SHADER:
      value = "GL_VERTEX_SHADER";
      break;
    case GL_TESS_CONTROL_SHADER:
      value = "GL_TESS_CONTROL_SHADER";
      break;
    case GL_GEOMETRY_SHADER:
      value = "GL_GEOMETRY_SHADER";
      break;
    case GL_FRAGMENT_SHADER:
      value = "GL_FRAGMENT_SHADER";
      break;
    case GL_TESS_EVALUATION_SHADER:
      value = "GL_TESS_EVALUATION_SHADER";
      break;
    default:
      break;
  }
  return value;
}

void ShaderManager::addFeedbackTransform(std::string nameVar) {
  const GLchar* feedbackVaryings[] = { nameVar.c_str() };
  glTransformFeedbackVaryings(shaderProg_, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
}

void ShaderManager::printActiveUniforms() {
  int curNumActiveUniforms = 0;
  glGetProgramiv(shaderProg_, GL_ACTIVE_UNIFORMS, &curNumActiveUniforms);
  if (curNumActiveUniforms == 0) {
    std::cout << "No uniforms" << std::endl;
  } else {
    for (int curUniform = 0; curUniform < curNumActiveUniforms; ++curUniform) {
      GLchar name[20];
      int name_len = -1, num = -1;
      GLenum type = GL_ZERO;
      glGetActiveUniform(shaderProg_, GLuint(curUniform), sizeof(name)-1, &name_len, &num, &type, name);

      std::cout << "Uniform number " << curUniform << ": " << name << std::endl;
    }

  }
}

//-------------------PRIVATE METHODS-----------------------------------------------------------------------------------------
GLuint ShaderManager::loadShaderFromFile(std::string path, GLenum shaderType) {
  GLuint shaderID = 0;
  //Open file
  std::string shaderString;
  std::ifstream sourceFile(path.c_str());

  if (sourceFile) {
    //Get shader source code and save it into a temp variable
    shaderString.assign((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
    const GLchar* shaderSource = shaderString.c_str();

    //Create shader ID
    shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, (const GLchar**) &shaderSource, NULL);
    //Compile shader source
    glCompileShader(shaderID);

    //Check shader for errors
    GLint shaderCompiled;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);

    if (shaderCompiled != GL_TRUE) {
      printf("Unable to compile shader %d!\n\nSource:\n%s\n", shaderID, shaderSource);
      GLchar InfoLog[1024];
      glGetShaderInfoLog(shaderID, 1024, NULL, InfoLog);
      fprintf(stderr, "Error compiling shader type %s: '%s'\n", getShaderTypeString(shaderType).c_str(), InfoLog);
      glDeleteShader(shaderID);
      shaderID = 0;
    }
  } else {
    printf("Unable to open file %s\n", path.c_str());
  }
  return shaderID;
}

