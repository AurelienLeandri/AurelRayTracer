#pragma once

#include <glm.hpp>

class Texture
{
public:
	Texture() = default;;
	virtual ~Texture() = default;;

public:
	virtual glm::vec3 color(float u, float v, const glm::vec3& p) const = 0;
};

