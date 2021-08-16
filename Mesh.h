#pragma once

#include "Shape.h"

#include <vector>

#include "Vertex.h"

class Transform;

/**
 * Representation of a triangle mesh. In the case you want a single-triangle mesh, consider using Triangle instead.
 */
class Mesh : public Shape
{
// Attributes
private:
    /**
     * Vertices of the mesh.
     */
    std::vector<Vertex> _vertices;

    /**
     * Indices referencing the vertices.
     */
    std::vector<int> _indices;

// Constructors/Destructors
public:
    /**
     * Creates an empty mesh.
     */
    Mesh();

// Inherited via Shape
public:
    virtual glm::vec3 sample(const HitRecord& record, float& pdf) const override;
    virtual float pdf(const glm::vec3& point, const HitRecord& record) const override;
    virtual void transform(const Transform& t) override;
    virtual float area() const override;

// Accessors
public:
    const std::vector<Vertex>& getVertices() const;
    std::vector<Vertex>& getVertices();
    const std::vector<int>& getIndices() const;
    std::vector<int>& getIndices();
};
