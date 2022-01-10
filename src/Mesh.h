#pragma once

#include "Shape.h"

#include <vector>

#include "Vertex.h"

class Transform;

class Mesh : public Shape
{
public:
    Mesh();

public:
    // Inherited via Shape
    virtual glm::vec3 sample(const HitRecord& record, float& pdf) const override;
    virtual float pdf(const glm::vec3& point, const HitRecord& record) const override;
    virtual void transform(const Transform& t) override;
    virtual float area() const override;
    virtual void commitGeometry(RTCDevice device, RTCScene rtcScene) override;

public:
    std::vector<Vertex> geometry;
    std::vector<int> indices;

};
