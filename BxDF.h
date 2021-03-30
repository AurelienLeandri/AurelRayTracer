#pragma once

#include <glm.hpp>

struct HitRecord;

class BxDF
{
public:
    enum Type {
        BSDF_REFLECTION = 1 << 0,
        BSDF_TRANSMISSION = 1 << 1,
        BSDF_DIFFUSE = 1 << 2,
        BSDF_GLOSSY = 1 << 3,
        BSDF_SPECULAR = 1 << 4,
        BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR |
        BSDF_REFLECTION | BSDF_TRANSMISSION,
    };

public:
    BxDF(int type);
    virtual ~BxDF();

public:
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const = 0;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3 &w_o, const HitRecord& hit_record, float &pdf) const = 0;

public:
    const int type;
};

