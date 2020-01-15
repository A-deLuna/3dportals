
#define GLM_ENABLE_EXPERIMENTAL

#include "camera.h"
#include "stdio.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "transform.h"

#include "GLFW/glfw3.h"

struct Camera {
  Transform transform;
  float yaw = 0;
  float pitch = 0;
};

static Camera camera = {};

static glm::vec2 previousMousePosition(-1,-1);

void initCamera() {
  camera.transform.rotation.w = 1;
  camera.transform.position = glm::vec3(0,2,0);
}

glm::mat4 camera_getVP() {
  glm::mat4 perspective =
      glm::perspective(glm::radians(45.f), 16.f / 9.f, .3f, 100.f);
  glm::mat4 rotation = glm::transpose(glm::toMat4(camera.transform.rotation));
  glm::mat4 translation = glm::translate(
      glm::mat4(1.f),
      -glm::vec3(camera.transform.position.x, camera.transform.position.y, camera.transform.position.z));
  glm::mat4 mvp = perspective * rotation * translation;
  return mvp;
}

bool insidePortalArea(glm::vec3 const* oldPosition, Transform const* portal1) {
  float minx = portal1->position.x - 1.f;
  float maxx = portal1->position.x + 1.f;
  float miny = portal1->position.y - 1.f;
  float maxy = portal1->position.y + 1.f;

  return minx <= oldPosition->x && oldPosition->x <= maxx
    && miny <= oldPosition->y && oldPosition->y <= maxy;
}

bool goingThroughPlane(glm::vec3 const* oldPosition, glm::vec3 const* movement, Transform const* portal) {
  float zAfterMove = oldPosition->z + movement->z;

  // correction is for near plane
  return oldPosition->z >= (portal->position.z + .3f) && zAfterMove <= (portal->position.z  +.3f);
}

void checkTeleport(glm::vec3 const* movement, Transform const* portal1, Transform const* portal2) {
  glm::vec3 oldPosition = camera.transform.position;
  camera.transform.position += *movement;
  // first check if we're standing in the x,y rectangular area where  we can teleport.
  if (insidePortalArea(&oldPosition, portal1)) {
    if (goingThroughPlane(&oldPosition, movement, portal1)) {
      glm::vec3 offsetFromPortal = camera.transform.position - portal1->position;
      camera.transform.position.x = portal2->position.x;
      camera.transform.position.z = portal2->position.z;
      camera.transform.position.x -= offsetFromPortal.x;
      camera.transform.position.z -= offsetFromPortal.z;
      camera.transform.rotation =
          glm::angleAxis(glm::radians(180.f), glm::vec3(0, 1.f, 0)) *
          camera.transform.rotation;
      camera.yaw +=180.f;
    }
  }
  if (insidePortalArea(&oldPosition, portal2)) {
    if (goingThroughPlane(&oldPosition, movement, portal2)) {
      printf("teleporting\n");
      glm::vec3 offsetFromPortal = camera.transform.position - portal2->position;
      camera.transform.position.x = portal1->position.x;
      camera.transform.position.z = portal1->position.z;
      camera.transform.position.x -= offsetFromPortal.x;
      camera.transform.position.z -= offsetFromPortal.z;
      camera.transform.rotation =
          glm::angleAxis(glm::radians(180.f), glm::vec3(0, 1.f, 0)) *
          camera.transform.rotation;
      camera.yaw +=180.f;
    }
  }
}

void updateCamera(GLFWwindow* window, Transform const* portal1, Transform const* portal2) {
  glm::vec3 localForward = camera.transform.rotation * glm::vec3(0, 0, -1);
  glm::vec3 forwardProjectedToGround = glm::normalize(glm::vec3(localForward.x, 0, localForward.z));

  glm::vec3 localUp = camera.transform.rotation * glm::vec3(0, 1, 0);
  glm::vec3 localRight = camera.transform.rotation * glm::vec3(1, 0, 0);
  glm::vec3 movement = {};
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    movement -= localRight * .01f;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    movement += localRight * .01f;
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    movement += forwardProjectedToGround * .01f;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    movement -= forwardProjectedToGround * .01f;
  }

  checkTeleport(&movement, portal1, portal2);

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

    camera.transform.rotation =
        glm::angleAxis(glm::radians(camera.yaw), glm::vec3(0,1,0));

    camera.transform.rotation =
        glm::angleAxis(glm::radians(camera.pitch), localRight) *
        camera.transform.rotation;
  }
}

void setCameraTransform(Transform const* transform) {
  camera.transform = *transform;
}

void getCameraTransform(Transform* transform) {
  *transform = camera.transform;
}
