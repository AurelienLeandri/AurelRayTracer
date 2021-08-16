#pragma once

#include "Shape.h"

/**
 * Representation of a sphere.
 */
class Sphere :
    public Shape
{
// Class-specific types
public:
    /**
     * Data of the sphere. Aligned to 16 bits for GPU-related operations like Embree's intersection acceleration or rendering.
     */
    struct alignas (16) Data {
        /**
         * Center of the sphere.
         */
        glm::vec3 center = glm::vec3(0, 0, 0);

        /**
         * Radius of the sphere.
         */
        float radius = 1.f;
    };

// Attributes
private:
    /**
     * Data of the sphere containing the center and the radius.
     */
    Data _data;
public:
    /**
     * Reference to the center of the sphere for convenience.
     */
    const glm::vec3& center = _data.center;

    /**
     * Reference to the radius of the sphere for convenience.
     */
    const float& radius = _data.radius;

// Constructors/Destructors
public:
    /**
     * Creates a sphere of center (0, 0, 0) and radius 1.
     */
    Sphere();

    /**
     * Creates a sphere.
     * \param center The center of the sphere
     * \param radius The radius of the sphere
     */
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

