#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct GLFWwindow;
struct Transform;

glm::mat4 camera_getVP();

void initCamera();

void updateCamera(GLFWwindow* window);

void setCameraTransform(Transform const* transform);
void getCameraTransform(Transform* transform);

