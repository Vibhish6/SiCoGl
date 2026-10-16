#pragma once
#include <glm/glm.hpp>
#include "Scene/SceneManager.hpp"

namespace g3d {
class Camera {
 public:
  glm::vec3 position{0.0f, 2.5f, 7.0f};
  float yaw{-90.0f};
  float pitch{-15.0f};
  float fov{45.0f};
  float nearPlane{0.05f};
  float farPlane{100.0f};
  glm::mat4 view() const;
  glm::mat4 projection(float aspect) const;
  glm::vec3 forward() const;
  Ray rayFromScreen(float xPixels, float yPixels, int width, int height) const;
};
}
