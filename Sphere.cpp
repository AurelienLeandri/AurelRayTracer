#include "Sphere.h"

#define _USE_MATH_DEFINES
#include <math.h>

Sphere::Sphere()
{
}

Sphere::Sphere(const glm::vec3& center, float radius)
    : Shape(), center(center), radius(radius)
{
}

float Sphere::area() const
{
    return 4 * (float)M_PI * radius * radius;
}

glm::vec3 Sphere::sample(const HitRecord& record, float& pdf) const
{
    return glm::vec3();
}

float Sphere::pdf(const glm::vec3& point, const HitRecord& record) const
{
    return 0.0f;
}

void Sphere::transform(const Transform& transform)
{
}
