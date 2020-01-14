#define GLEW_STATIC
#include "GL/glew.h"
#include "stddef.h"
#include "shaders.h"
#include "camera.h"
#include "glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include "box.h"
#include "transform.h"

// clang-format off
float colors[] = {
  0.0,1.0,0.0, // green
  0.0,0.0,1.0, // blue
  1.0,1.0,1.0, // white
  0.0,1.0,0.0, // green
  0.0,1.0,1.0, // yellow
  1.0,1.0,1.0, // white
  0.0,1.0,0.0, // green
  0.0,0.0,1.0, // blue
  1.0,0.5,0.5, // pink
  0.0,1.0,0.0, // green
  0.0,1.0,1.0, // yellow
  1.0,0.5,0.5, // pink
  1.0,0.0,0.0, // red
  0.0,0.0,1.0, // blue
  1.0,1.0,1.0, // white
  1.0,0.0,0.0, // red
  0.0,1.0,1.0, // yellow
  1.0,1.0,1.0, // white
  1.0,0.0,0.0, // red
  0.0,0.0,1.0, // blue
  1.0,0.5,0.5, // pink
  1.0,0.0,0.0, // red
  0.0,1.0,1.0, // yellow
  1.0,0.5,0.5, // pink
};
static float positions[] = {
  -1.f,-1.f,-1.f, // 0  0 left bot back green
  -1.f,-1.f,-1.f, // 0  1 left bot back blue
  -1.f,-1.f,-1.f, // 0  2 left bot back white
  -1.f,-1.f, 1.f, // 1  3 left bot front green
  -1.f,-1.f, 1.f, // 1  4 left bot front yellow
  -1.f,-1.f, 1.f, // 1  5 left bot front white
  -1.f, 1.f,-1.f, // 2  6 left top back green
  -1.f, 1.f,-1.f, // 2  7 left top back blue
  -1.f, 1.f,-1.f, // 2  8 left top back pink
  -1.f, 1.f, 1.f, // 3  9 left top front green
  -1.f, 1.f, 1.f, // 3 10 left top front yellow
  -1.f, 1.f, 1.f, // 3 11 left top front pink
   1.f,-1.f,-1.f, // 4 12 right bot back red
   1.f,-1.f,-1.f, // 4 13 right bot back blue
   1.f,-1.f,-1.f, // 4 14 right bot back white
   1.f,-1.f, 1.f, // 5 15 right bot front red
   1.f,-1.f, 1.f, // 5 16 right bot front yellow
   1.f,-1.f, 1.f, // 5 17 right bot front white
   1.f, 1.f,-1.f, // 6 18 right top back red
   1.f, 1.f,-1.f, // 6 19 right top back blue
   1.f, 1.f,-1.f, // 6 20 right top back pink
   1.f, 1.f, 1.f, // 7 21 right top front red
   1.f, 1.f, 1.f, // 7 22 right top front yellow
   1.f, 1.f, 1.f, // 7 23 right top front pink
};

static GLuint indices[] = {
  // green
  0, 6, 9, // left back
  9, 3, 0, // left front
  // red
  18, 12, 15, // right back
  15, 21, 18, // rightront
  // blue
  19, 7, 1, // back lt
  1, 13, 19, // back right
  // yellow
  4, 10, 22, //ront lt
  22, 16, 4, //ront right
  // pink
  11, 8, 20, // top lt
  20, 23, 11, // top right
  // white 
  14, 2, 5, // bot lt
  5, 17, 14, // bot right
};
// clang-format on

static size_t n_indices = sizeof(indices) / sizeof(indices[0]);

static GLuint vao;
static GLuint program;

static void setupCubeVao() {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint bufferIds[3];
  glGenBuffers(3, bufferIds);
  GLuint positionBuffer = bufferIds[0];
  GLuint colorBuffer = bufferIds[1];
  GLuint indexBuffer = bufferIds[2];

  // bind index positions
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  // enable positions in the vertex shader.
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
  // enable positions in the vertex shader.
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
}

struct uniforms {
  GLint worldPosition;
  GLint mvp;
};

static uniforms uniforms = {};

static void setupShader() {
  program = shaders_getProgram("vertex.glsl", "fragment.glsl");
  uniforms.mvp = glGetUniformLocation(program, "mvp");
}

void setupCube() {
  setupCubeVao();
  setupShader();
}

void drawCube(Transform const* transform) {
  glUseProgram(program);
  glBindVertexArray(vao);

  glm::mat4 vp = camera_getVP();
  glm::mat4 mvp = vp * glm::translate(transform->position);
  glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
  glDrawElements(GL_TRIANGLES, n_indices, GL_UNSIGNED_INT, NULL);
}
