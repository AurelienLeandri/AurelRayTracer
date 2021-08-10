#pragma once

#include <glm/glm.hpp>
#include <array>

struct alignas (16) Vertex {
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 normal = glm::vec3(1, 0, 0);
	glm::vec2 uv = glm::vec2(0, 0);
};


