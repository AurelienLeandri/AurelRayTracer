#include "BSDF.h"

#include "BxDF.h"

#include "Utils.h"
#include "HitRecord.h"

BSDF::BSDF()
{
}


BSDF::~BSDF()
{
    for (const BxDF * bxdf : _bxdfs) {
        delete bxdf;
    }
    _bxdfs.clear();
}

glm::vec3 BSDF::f(const glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord &hit_record) const
{
    glm::vec3 f_total = glm::vec3();
    for (const BxDF *bxdf : _bxdfs) {
        f_total += bxdf->f(w_i, w_o, hit_record);
    }
    return f_total;
}

glm::vec3 BSDF::sample_f(glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord& hit_record) const
{
    size_t random_index = static_cast<unsigned int>(frand() * (_bxdfs.size() - 1));
    return _bxdfs[random_index]->sample_f(w_i, w_o, hit_record);
}

bool BSDF::add(const BxDF * bxdf)
{
    if (_bxdfs.size() >= MAX_NB_BXDFS)
        return false;
    _bxdfs.push_back(bxdf);
    return true;
}
