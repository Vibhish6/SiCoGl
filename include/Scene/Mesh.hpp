#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace g3d {
struct Vertex { glm::vec3 position; glm::vec3 normal; glm::vec2 uv; };
struct MeshData { std::vector<Vertex> vertices; std::vector<unsigned int> indices; };
struct Mesh { MeshData data; unsigned int vao{0}, vbo{0}, ebo{0}; bool uploaded{false}; };
using MeshPtr = std::shared_ptr<Mesh>;
}
