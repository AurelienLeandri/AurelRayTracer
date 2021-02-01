#pragma once

#include <glm.hpp>

class Material;
class BSDF;

typedef struct HitRecord
{
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 normal = glm::vec3(1, 0, 0);
    float t = 0.f;
    float u = 0.f;
    float v = 0.f;
    const Material* material = nullptr;
    const BSDF* bsdf = nullptr;
    glm::vec3 emission = glm::vec3(0, 0, 0);
} HitRecord;

