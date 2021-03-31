#include "BSDF.h"

#include "Utils.h"
#include "HitRecord.h"

BSDF::BSDF()
{
}


BSDF::~BSDF()
{
}

// NOTE: Both BSDF::f and BSDF::sample_f assume that w_o (and w_i for BSDF::f) are oriented in the same way as the normal and are normalized.

glm::vec3 BSDF::f(const glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord &hit_record) const
{
    glm::vec3 w_i_local = hit_record.shadingCoordinateSystem * w_i;
    glm::vec3 w_o_local = hit_record.shadingCoordinateSystem * w_o;
    bool reflected = w_o_local.z * w_i_local.z > 0;  // Both w_i and w_o are on the same side of the surface -> Reflection. If not -> Transmission.
    glm::vec3 f(0, 0, 0);
    for (int i = 0; i < _bxdfs.size(); ++i) {
        if ((reflected && (_bxdfs[i]->type & BxDF::BSDF_REFLECTION)) || (!reflected && (_bxdfs[i]->type & BxDF::BSDF_TRANSMISSION))) {
            f += _bxdfs[i]->f(w_i_local, w_o_local, hit_record);
        }
    }
    return f;
}

glm::vec3 BSDF::sample_f(glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord& hit_record, float &pdf) const
{
    glm::vec3 w_o_local = hit_record.shadingCoordinateSystem * w_o;
    size_t random_index = static_cast<unsigned int>(frand() * (_bxdfs.size() - 1));
    glm::vec3 f = _bxdfs[random_index]->sample_f(w_i, w_o_local, hit_record, pdf);
    w_i = glm::transpose(hit_record.shadingCoordinateSystem) * w_i;
    return f;
}

bool BSDF::add(std::shared_ptr<const BxDF> bxdf)
{
    if (_bxdfs.size() >= MAX_NB_BXDFS)
        return false;
    _bxdfs.push_back(bxdf);
    return true;
}
