#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct GLFWwindow;
struct Transform;

glm::mat4 camera_getVP();

void initCamera();

void updateCamera(GLFWwindow* window, Transform const* portal1, Transform const* portal2); 

void setCameraTransform(Transform const* transform);
void getCameraTransform(Transform* transform);

