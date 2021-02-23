#pragma once

#include <glm.hpp>

#include <vector>

class BxDF;
struct HitRecord;

class BSDF
{
public:
    BSDF();
    ~BSDF();

public:
    glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord &hit_record) const;
    glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3 &w_o, const HitRecord &hit_record) const;
    bool add(const BxDF *bxdf);  // TODO: maybe make unique, or shared depending on future needs

private:
    std::vector<const BxDF *> _bxdfs;

private:
    static const size_t MAX_NB_BXDFS = 8;
};

