
#define GLM_ENABLE_EXPERIMENTAL
#define GLEW_STATIC

#include <glm/glm.hpp>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "box.h"
#include "camera.h"
#include "ground.h"
#include "stdio.h"
#include "wall.h"

GLFWwindow* window;

void init() {
  glfwInit();

  window = glfwCreateWindow(1280, 720, "hello world", NULL, NULL);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  glewInit();

  // version info
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);

  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);
}

struct Transform {
  glm::vec3 position;
  glm::vec3 scale;
  glm::quat rotation;
};

static uint32_t next_object_id = 0;
static Transform transforms[1000];

uint32_t createTransform() {
  transforms[next_object_id].scale = glm::vec3(1, 1, 1);
  transforms[next_object_id].rotation.w = 1;
  return next_object_id++;
}

int main() {
  init();
  setupCube();
  setupGround();
  //setupWall();
  glClearColor(0.8f, .8f, .8f, 1.f);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);

  glEnable(GL_DEPTH_TEST);

  initCamera();

  uint32_t cube_1 = createTransform();
  transforms[cube_1].position = glm::vec3(0, 1, -1);

  uint32_t ground = createTransform();
  transforms[ground].position = glm::vec3(0, -2, -1);
  transforms[ground].scale = glm::vec3(5, 1, 5);

  uint32_t wall_1 = createTransform();
  transforms[ground].position = glm::vec3(0, .5, -1);

  while (!glfwWindowShouldClose(window)) {
    // TODO have a real input system instead of passing window* all over the
    // place.
    updateCamera(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawCube(&transforms[cube_1]);

    drawGround(&transforms[ground]);

    //drawWall(&transforms[wall_1]);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
