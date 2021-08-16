#include "Sphere.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Transform.h"

// Constructors/Destructors

Sphere::Sphere()
	: Shape(Shape::Type::SPHERE)
{
}

Sphere::Sphere(const glm::vec3& center, float radius)
    : Shape(Shape::Type::SPHERE)
{
	_data.center = center;
	_data.radius = radius;
}

// Inherited via Shape

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
    _data.center = glm::vec3(transform.getMatrix() * glm::vec4(center, 1));
}

float Sphere::area() const
{
    return 4 * (float)M_PI * radius * radius;
}

// Getters/Setters

const Sphere::Data& Sphere::getData() const
{
    return _data;
}

