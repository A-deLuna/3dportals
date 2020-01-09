#define GLEW_STATIC
#include "shaders.h"

#include <vector>

#include "GL/glew.h"
#include "stdio.h"

static std::vector<char> getFileContents(const char* fileName) {
  FILE* file = fopen(fileName, "r");
  if (file == NULL) {
    printf("error reading file %s\n", fileName);
  }

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  fseek(file, 0, SEEK_SET);
  std::vector<char> contents(size + 1);
  fread(contents.data(), 1, size, file);
  contents[size] = '\0';
  return contents;
}

static GLuint createShader(const char* fileName, GLenum shaderType) {
  std::vector<char> content = getFileContents(fileName);
  const char* contentPtr = content.data();
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &contentPtr, NULL);
  glCompileShader(shader);

  GLint logSize;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

  if (logSize != 0) {
    std::vector<char> log(logSize);
    glGetShaderInfoLog(shader, logSize, NULL, log.data());
    printf("shader log for file %s:\n%s\n", fileName, log.data());
  }
  return shader;
}

GLuint shaders_getProgram(char const* vertShaderFilename,
                          char const* fragShaderFilename) {
  GLuint vertShader = createShader(vertShaderFilename, GL_VERTEX_SHADER);
  GLuint fragShader = createShader(fragShaderFilename, GL_FRAGMENT_SHADER);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);
  GLint logSize;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
  if (logSize != 0) {
    std::vector<char> log(logSize);
    glGetProgramInfoLog(program, logSize, NULL, log.data());
    printf("program log: %s\n", log.data());
  }
  return program;
}
