#pragma once

#include <glm/glm.hpp>

#include "BxDF.h"

struct HitRecord;

class BxDF
{
public:
    class Type {
    public:
        Type(int flags);

    public:
        enum Flags {
            BXDF_NONE = 0,
            BXDF_REFLECTION = 1 << 0,
            BXDF_TRANSMISSION = 1 << 1,
            BXDF_DIFFUSE = 1 << 2,
            BXDF_GLOSSY = 1 << 3,
            BXDF_SPECULAR = 1 << 4,
            BXDF_ALL = BXDF_DIFFUSE | BXDF_GLOSSY | BXDF_SPECULAR |
            BXDF_REFLECTION | BXDF_TRANSMISSION,
        };

    public:
        bool isType(Type type2) const;

    public:
        int flags = 0;
    };

public:
    BxDF(Type type);
    virtual ~BxDF();

public:
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const = 0;
    virtual float pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3 &w_o, const HitRecord& hit_record, float &pdf) const;

public:
    const Type type;
};

bool operator==(const BxDF::Type& lhs, const BxDF::Type& rhs);
bool operator!=(const BxDF::Type& lhs, const BxDF::Type& rhs);


