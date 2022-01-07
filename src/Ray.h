#pragma once

#include <glm/glm.hpp>

class Ray
{
public:
    Ray();
    Ray(const glm::vec3 &origin, const glm::vec3 &direction);

public:
    glm::vec3 pointAtParameter(float t) const;

public:
    glm::vec3 origin = glm::vec3(0);
    glm::vec3 direction = glm::vec3(0, 0, 1);
    mutable float eta = 1;  // Should be replaced by a more complete mediu class when we tackle volume rendering
};

class RayDifferential : public Ray {
public:
    RayDifferential();
    RayDifferential(const glm::vec3& origin, const glm::vec3& direction);

public:
    void scaleDifferentials(float weight);

public:
    bool hasDifferentials = false;
    glm::vec3 rxOrigin = glm::vec3(0), ryOrigin = glm::vec3(0);
    glm::vec3 rxDirection = glm::vec3(0, 0, 1), ryDirection = glm::vec3(0, 0, 1);

};
