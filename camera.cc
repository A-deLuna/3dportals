
#define GLM_ENABLE_EXPERIMENTAL

#include "camera.h"
#include "stdio.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "GLFW/glfw3.h"

struct Camera {
  glm::vec3 position;
  glm::quat rotation;
  float yaw = 0;
  float pitch = 0;
};

static Camera camera = {};

static glm::vec2 previousMousePosition(-1,-1);

void initCamera() {
  camera.rotation = glm::angleAxis(glm::radians(0.f), glm::vec3(0,1, 0));
}

glm::mat4 camera_getVP() {
  glm::mat4 perspective =
      glm::perspective(glm::radians(45.f), 16.f / 9.f, .3f, 50.f);
  glm::mat4 rotation = glm::transpose(glm::toMat4(camera.rotation));
  glm::mat4 translation = glm::translate(
      glm::mat4(1.f),
      -glm::vec3(camera.position.x, camera.position.y, camera.position.z));
  glm::mat4 mvp = perspective * rotation * translation;
  return mvp;
}

void updateCamera(GLFWwindow* window) {
  glm::vec3 localForward = camera.rotation * glm::vec3(0, 0, -1);
  glm::vec3 forwardProjectedToGround = glm::normalize(glm::vec3(localForward.x, 0, localForward.z));

  glm::vec3 localUp = camera.rotation * glm::vec3(0, 1, 0);
  glm::vec3 localRight = camera.rotation * glm::vec3(1, 0, 0);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.position -= localRight * .01f;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.position += localRight * .01f;
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.position += forwardProjectedToGround * .01f;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.position -= forwardProjectedToGround * .01f;
  }

  double x, y;
  glm::vec2 currentMousePosition;
  glfwGetCursorPos(window, &x, &y);
  currentMousePosition = glm::vec2(x, y);

  if (previousMousePosition == glm::vec2(-1,-1)) {
    previousMousePosition = currentMousePosition;
  }

  glm::vec2 mouseDelta = currentMousePosition - previousMousePosition;

  previousMousePosition = currentMousePosition;
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    camera.yaw += -mouseDelta.x * .1f;
    camera.pitch += mouseDelta.y * .1f;

    camera.rotation =
        glm::angleAxis(glm::radians(camera.yaw), glm::vec3(0,1,0));

    camera.rotation =
        glm::angleAxis(glm::radians(camera.pitch), localRight) *
        camera.rotation;
  }
}
