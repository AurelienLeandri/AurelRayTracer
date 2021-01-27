#pragma once

#include <glm.hpp>

class Material2;
class BSDF;

typedef struct HitRecord
{
    glm::vec3 position;
    glm::vec3 normal;
    float t = 0.f;
    float u = 0.f;
    float v = 0.f;
    const Material* material = nullptr;
    const BSDF* bsdf = nullptr;
    glm::vec3 emission;
} hit_record;

