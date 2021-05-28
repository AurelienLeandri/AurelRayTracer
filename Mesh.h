#pragma once

#include <vector>
#include "Vertex.h"

struct Transform;


// TODO: make abstract
class Mesh
{
public:
    virtual float sample(glm::vec3& value, const glm::vec3& origin, const glm::vec3& origin_normal) const { return 0; }
    
    virtual float pdf(const glm::vec3& value, const glm::vec3& origin, const glm::vec3& origin_normal) const {
        return std::fabs(glm::dot(value, value)) / (std::fabs(glm::dot(glm::normalize(value), origin_normal)) * (area() * 0.5f));
    }

    void transform(const Transform& t);
    virtual float area() const { return 0; };

public:
    std::vector<Vertex> geometry;
    std::vector<unsigned int> indices;
    unsigned int materialId = 0;
};
