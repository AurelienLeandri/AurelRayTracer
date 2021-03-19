#pragma once

#include <glm.hpp>

class Vertex
{
public:
	Vertex() {}
	Vertex(glm::vec3 position, glm::vec2 uv = glm::vec2(0, 0), glm::vec3 normal = glm::vec3(1, 0, 0))
		: position(position), uv(uv), normal(normal)
	{}

public:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec2 uv = glm::vec2(0, 0);
	glm::vec3 normal = glm::vec3(1, 0, 0);
};

