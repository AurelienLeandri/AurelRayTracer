#pragma once

#include <glm/glm.hpp>

#include "Ray.h"
#include "BSDF.h"

class Material;

typedef struct HitRecord
{
    glm::vec3 position = glm::vec3(0);
    glm::vec3 normal = glm::vec3(0, 0, 1);
    glm::vec3 tangent = glm::vec3(1, 0, 0);
    glm::vec3 bitangent = glm::vec3(0, 1, 0);
    float tRay = 0.f;
    float u = 0.f;
    float v = 0.f;
    const Material* material = nullptr;
    BSDF bsdf;
    glm::vec3 emission = glm::vec3(0);
    glm::mat3 shadingCoordinateSystem = glm::mat3(1);
    Ray ray;
    int shapeId = 0;
} HitRecord;

