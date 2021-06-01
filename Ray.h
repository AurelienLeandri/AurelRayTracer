#pragma once

#include <glm/glm.hpp>

class Ray
{
public:
    Ray(float time = 0.f);
    Ray(const glm::vec3 &origin, const glm::vec3 &direction, float time = 0.f);

public:
    glm::vec3 pointAtParameter(float t) const;

public:
    glm::vec3 origin;
    glm::vec3 direction;
	float time;
    mutable float eta = 1;  // Should be replaced by a more complete mediu class when we tackle volume rendering
};

class RayDifferential : public Ray {
public:
    void scaleDifferentials(float weight);

};
