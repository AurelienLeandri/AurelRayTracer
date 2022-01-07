
#include "Ray.h"


Ray::Ray() : origin(), direction(0, 0, 1)
{
}

Ray::Ray(const glm::vec3 & origin, const glm::vec3 & direction) : origin(origin), direction(direction)
{
}

glm::vec3 Ray::pointAtParameter(float t) const
{
    return origin + direction * t;
}

RayDifferential::RayDifferential() : Ray()
{
}

RayDifferential::RayDifferential(const glm::vec3& origin, const glm::vec3& direction)
    : Ray(origin, direction)
{
}

void RayDifferential::scaleDifferentials(float weight)
{
    rxOrigin = origin + (rxOrigin - origin) * weight;
    ryOrigin = origin + (ryOrigin - origin) * weight;
    rxDirection = direction + (rxDirection - direction) * weight;
    ryDirection = direction + (ryDirection - direction) * weight;
}
