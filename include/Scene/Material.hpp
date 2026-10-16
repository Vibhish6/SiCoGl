#pragma once
#include <glm/glm.hpp>

namespace g3d {
enum class MaterialType { Matte, Glossy, Metallic, Glass, Emissive };
struct Material {
  MaterialType type{MaterialType::Matte};
  glm::vec3 baseColor{0.25f, 0.65f, 0.95f};
  float metallic{0.0f};
  float roughness{0.55f};
  float alpha{1.0f};
  glm::vec3 emissive{0.0f};
  float shininess() const { return 2.0f + (1.0f - roughness) * 126.0f; }
};
}
