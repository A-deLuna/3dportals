#define GLEW_STATIC
#include "shaders.h"

#include <vector>

#include <unordered_map>
#include <utility>
#include "GL/glew.h"
#include "stdio.h"

namespace std {
template <>
struct hash<std::pair<char const*, char const*>> {
  size_t operator()(const std::pair<char const*, char const*>& x) const {
    uint32_t s = sizeof(size_t);
    uint32_t half = s / 2;
    uint32_t hash = 0;
    uintptr_t first = (uintptr_t)x.first;
    uintptr_t second = (uintptr_t)x.second;
    hash = first & ((1u << half) - 1u);
    hash <<= half;
    hash = hash | (second & ((1u << half) - 1u));
    return hash;
  }
};
}  // namespace std

static std::unordered_map<std::pair<char const*, char const*>, GLuint>
    shadersToProgram(100);

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
  std::pair<char const*, char const*> pair =
      std::make_pair(vertShaderFilename, fragShaderFilename);
  if (shadersToProgram.count(pair)) {
    return shadersToProgram[pair];
  }
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
  shadersToProgram[pair] = program;
  return program;
}
