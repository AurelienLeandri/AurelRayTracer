
#include "Ray.h"


Ray::Ray(float time) : time(time), origin(), direction(0, 0, 1)
{
}

Ray::Ray(const glm::vec3 & origin, const glm::vec3 & direction, float time) : origin(origin), direction(direction), time(time)
{
}

glm::vec3 Ray::pointAtParameter(float t) const
{
    return origin + direction * t;
}
