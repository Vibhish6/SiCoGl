#pragma once
#include "Material.hpp"
#include "Mesh.hpp"
#include "Transform.hpp"
#include <glm/glm.hpp>
#include <cstdint>
#include <string>

namespace g3d {
enum class PrimitiveType { Cube, Sphere, Cylinder, Cone, Plane, Torus };
struct SceneObject {
  std::uint64_t id{0};
  std::string name;
  PrimitiveType primitive{PrimitiveType::Cube};
  Transform transform;
  Material material;
  MeshPtr mesh;
  glm::vec3 localHalfExtents{0.5f};
  bool selected{false};
  bool visible{true};
  bool locked{false};
};
}
