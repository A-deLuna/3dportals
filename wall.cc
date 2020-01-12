#include "wall.h"
#define GLEW_STATIC
#include "GL/glew.h"
#include "shaders.h"
#include "stb_image.h"
#include "camera.h"
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include "wall.h"
#include "transform.h"

// clang-format off
static float positions[] = {
  -1.f, -1.f, 0.f,
  -1.f,  1.f, 0.f,
   1.f, -1.f, 0.f,
   1.f,  1.f, 0.f,
};

static float texture_coords[] = {
  0.f, 0.f,
  0.f, 1.f,
  1.f, 0.f,
  1.f, 1.f,
};

static GLuint indices[] = {
  0,2,1,
  2,3,1
};
// clang-format on

static size_t n_indices = sizeof(indices) / sizeof(indices[0]);


static GLuint vao;
static GLuint program;

static void setupWallVao() {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint bufferIds[3];
  glGenBuffers(3, bufferIds);
  GLuint positionBuffer = bufferIds[0];
  GLuint textureBuffer = bufferIds[1];
  GLuint indexBuffer = bufferIds[2];

  // bind index positions
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // bind position buffer
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  // bind texture coords
  glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coords), texture_coords,
               GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  GLuint tex;
  glGenTextures(1, &tex);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);

  int width, height, nChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load("assets/FloorTile/FloorTileDiffuse.png",
                                  &width, &height, &nChannels, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);
  glGenerateMipmap(GL_TEXTURE_2D);
}

struct uniforms {
  GLint mvp;
  GLint texture;
  GLint textureScale;
};

static uniforms uniforms = {};

static void setupShader() {
  program = shaders_getProgram("texture.vert", "texture.frag");
  uniforms.mvp = glGetUniformLocation(program, "mvp");
  uniforms.texture = glGetUniformLocation(program, "myTexture");
  uniforms.textureScale = glGetUniformLocation(program, "textureScale");
}

void setupWall() {
  setupWallVao();
  setupShader();
}

void drawWall(Transform const* transform) {
  glUseProgram(program);
  glBindVertexArray(vao);

  glm::mat4 mvp = camera_getVP() * glm::translate(transform->position) *
                  glm::scale(transform->scale);
  glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
  glUniform1f(uniforms.textureScale, 5.f);

  glUniform1i(uniforms.texture, 1);
  glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_INT, NULL);
}
