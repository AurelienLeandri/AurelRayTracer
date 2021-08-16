#pragma once

#include "Shape.h"

class Sphere :
    public Shape
{
// Class-specific types
public:
    struct alignas (16) Data {
        glm::vec3 center = glm::vec3(0, 0, 0);
        float radius = 1.f;
    };

// Attributes
private:
    Data _data;
public:
    const glm::vec3& center = _data.center;
    const float& radius = _data.radius;

// Constructors/Destructors
public:
    Sphere();
    Sphere(const glm::vec3& center, float radius);

// Inherited via Shape
public:
    virtual float area() const override;
    virtual glm::vec3 sample(const HitRecord& record, float& pdf) const override;
    virtual float pdf(const glm::vec3& point, const HitRecord& record) const override;
    virtual void transform(const Transform& transform) override;

// Accessors
public:
    const Data& getData() const;

};

