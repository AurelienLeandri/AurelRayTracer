#pragma once

#include <glm/glm.hpp>

class Film
{
public:
	const glm::ivec2& getDimensions() const;
	void addSample(const glm::ivec2 position, const glm::vec3& radiance, float weight);
	void writeImage();

private:
	glm::ivec2 _dimensions = glm::ivec2(600, 600);
};

