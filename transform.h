#pragma once

#include <glm/fwd.hpp>

struct Transform {
  glm::vec3 position; 
  glm::vec3 scale; 
  glm::quat rotation; 
};
