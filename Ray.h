#pragma once

#include <glm.hpp>

class Ray
{
public:
    Ray(float time = 0.f);
    Ray(const glm::vec3 &origin, const glm::vec3 &direction, float time = 0.f);

    glm::vec3 pointAtParameter(float t) const;

    glm::vec3 origin;
    glm::vec3 direction;
	float time;
};

