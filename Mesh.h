#pragma once

#include <vector>
#include "Vertex.h"

struct Transformation {
    glm::vec3 translation = glm::vec3(0, 0, 0);
    glm::vec3 rotation_rads = glm::vec3(0, 0, 0);
    glm::vec3 scaling = glm::vec3(1, 1, 1);
};

class Mesh
{
public:
    virtual float sample(glm::vec3& value, const glm::vec3& origin, const glm::vec3& origin_normal) const { return 0; }
    void transform(const Transformation& t);

public:
    std::vector<Vertex> geometry;
    std::vector<unsigned int> indices;
    unsigned int materialId = 0;
};
