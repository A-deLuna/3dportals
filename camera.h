#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct GLFWwindow;

glm::mat4 camera_getVP();

void initCamera();

void updateCamera(GLFWwindow* window);
