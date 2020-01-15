
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#define GLEW_STATIC

#include <glm/glm.hpp>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "box.h"
#include "camera.h"
#include "ground.h"
#include "stdio.h"
#include "portal.h"
#include "wall.h"
#include "transform.h"

GLFWwindow* window;
std::vector<uint32_t> scene(100);

void init() {
  glfwInit();

  window = glfwCreateWindow(1280, 720, "hello world", NULL, NULL);
  
  const GLFWvidmode* videoMode =
    glfwGetVideoMode(glfwGetPrimaryMonitor());
  glfwSetWindowPos(window,videoMode->width /2 - 320 , videoMode->height / 2 - 720);

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


static uint32_t next_object_id = 0;
static Transform transforms[1000];
static void(*drawFns[1000])(Transform const*);

uint32_t createTransform() {
  transforms[next_object_id].scale = glm::vec3(1, 1, 1);
  transforms[next_object_id].rotation.w = 1;
  return next_object_id++;
}

void moveCameraToBackOfPortal(Transform* transform, uint32_t frontPortal, uint32_t backPortal) {
    Transform copyTransform = *transform;

    // take vector from center of portal to camera position.
    glm::vec3 portalToCamera = transform->position - transforms[frontPortal].position;

    // rotate vector towards world forward by inversely rotating the portal rotation
    glm::vec3 offsetForward = glm::inverse(transforms[frontPortal].rotation) * portalToCamera;

    // Now, first rotate the offset vector around the portal direction, then
    // rotate 180 degrees around the y axis
    glm::vec3 offsetBackOfOtherPortal = transforms[backPortal].rotation * offsetForward;
    offsetBackOfOtherPortal = glm::angleAxis(glm::radians(180.f), glm::vec3(0,1,0)) * offsetBackOfOtherPortal;
    copyTransform.position = transforms[backPortal].position + offsetBackOfOtherPortal;

    // Hack, rotate camera by 180 degrees in y axis to simulate camera moving
    copyTransform.rotation =  glm::angleAxis(glm::radians(180.f), glm::vec3(0,1,0)) * copyTransform.rotation;

    setCameraTransform(&copyTransform);

}

void drawScene() {
  for (uint32_t id : scene) {
    drawFns[id](&transforms[id]);
  }
}

int main() {
  init();
  setupCube();
  setupGround();
  setupWall();
  setupPortal();

  glClearColor(0.0f, .0f, .0f, 1.f);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);

  glEnable(GL_DEPTH_TEST);

  initCamera();

  uint32_t cube_1 = createTransform();
  transforms[cube_1].position = glm::vec3(-1, 1.f, -2);
  drawFns[cube_1] = drawCube;
  scene.push_back(cube_1);

  uint32_t cube_2 = createTransform();
  transforms[cube_2].position = glm::vec3(3, 1.f, -2);
  drawFns[cube_2] = drawCube;
  scene.push_back(cube_2);

  uint32_t cube_3 = createTransform();
  transforms[cube_3].position = glm::vec3(-25, 1.f, 10);
  transforms[cube_3].rotation = glm::angleAxis(glm::radians(90.f), glm::normalize(glm::vec3(1,1,1)));
  drawFns[cube_3] = drawCube;
  scene.push_back(cube_3);

  uint32_t ground = createTransform();
  transforms[ground].position = glm::vec3(0, 0, -1);
  transforms[ground].scale = glm::vec3(50, 1, 50);
  drawFns[ground] = drawGround;
  scene.push_back(ground);

  uint32_t wall_1 = createTransform();
  transforms[wall_1].position = glm::vec3(0, .5, -10.f);
  transforms[wall_1].scale = glm::vec3(50, 7, 1);
  transforms[wall_1].rotation =
      glm::angleAxis(glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
  drawFns[wall_1] = drawWall;
  scene.push_back(wall_1);

  uint32_t portal_1 = createTransform();
  transforms[portal_1].position = glm::vec3(0, 2.f, -9.98f);
  transforms[portal_1].rotation =
      glm::angleAxis(glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));

  uint32_t portal_2 = createTransform();
  transforms[portal_2].position = glm::vec3(-24, 2.f, -9.98f);
  transforms[portal_2].rotation =
      glm::angleAxis(glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));


  while (!glfwWindowShouldClose(window)) {
    // TODO have a real input system instead of passing window* all over the
    // place.
    updateCamera(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    drawScene();

    // write to stencil buffer
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xff);
    drawPortal(&transforms[portal_1]);
    // stop writing to stencil buffer
    // only draw where stencil value equal to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 1, 0xff);

    // clear depth buffer so that portals are rendered
    glClear(GL_DEPTH_BUFFER_BIT);

    // Grab camera position
    Transform cachedCameraTrans;
    getCameraTransform(&cachedCameraTrans);
    moveCameraToBackOfPortal(&cachedCameraTrans, portal_1, portal_2);

    drawScene();
    // clear stencil buffer
    glClear(GL_STENCIL_BUFFER_BIT);

    // move camera back to original position
    setCameraTransform(&cachedCameraTrans);

    // write to stencil buffer
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xff);

    drawPortal(&transforms[portal_2]);

    // stop writing to stencil buffer
    // only draw where stencil value equal to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 1, 0xff);

    // clear depth buffer so that portals are rendered
    glClear(GL_DEPTH_BUFFER_BIT);

    moveCameraToBackOfPortal(&cachedCameraTrans, portal_2, portal_1);

    drawScene();

    // disable stenil test
    glDisable(GL_STENCIL_TEST);

    // move camera back to original position
    setCameraTransform(&cachedCameraTrans);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

