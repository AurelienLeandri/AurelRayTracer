#pragma once

#include "Shape.h"

class Sphere :
    public Shape
{
public:
    Sphere();
    Sphere(const glm::vec3& center, float radius);

public:
    // Inherited via Shape
    virtual float area() const override;
    virtual glm::vec3 sample(const HitRecord& record, float& pdf) const override;
    virtual float pdf(const glm::vec3& point, const HitRecord& record) const override;
    virtual void transform(const Transform& transform) override;

public:
    glm::vec3 center = glm::vec3(0, 0, 0);
    float radius = 1.f;
};

