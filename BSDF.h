#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "BxDF.h"

struct HitRecord;

class BSDF
{
public:
    BSDF();
    ~BSDF();

public:
    glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const;
    float pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord &hit_record) const;
    glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3 &w_o, const HitRecord &hit_record, float &pdf, BxDF::Type& sampled_type, const BxDF::Type& bxdfTypes = BxDF::Type::BXDF_ALL) const;
    bool add(std::shared_ptr<const BxDF> bxdf);
    int nbMatchingComponents(BxDF::Type type) const;

private:
    std::vector<std::shared_ptr<const BxDF>> _bxdfs;

private:
    static const size_t MAX_NB_BXDFS = 8;
};

