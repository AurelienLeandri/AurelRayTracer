#pragma once

#include <vector>
#include "Vertex.h"

struct Transform;

class Mesh
{
public:
    virtual float sample(glm::vec3& value, const glm::vec3& origin, const glm::vec3& origin_normal) const { return 0; }
    void transform(const Transform& t);

public:
    std::vector<Vertex> geometry;
    std::vector<unsigned int> indices;
    unsigned int materialId = 0;
};
