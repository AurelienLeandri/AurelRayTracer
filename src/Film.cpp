#include "Film.h"

const glm::ivec2& Film::getDimensions() const
{
	return _dimensions;
}

void Film::addSample(const glm::ivec2 position, const glm::vec3& radiance, float weight)
{
}

void Film::writeImage()
{
}
