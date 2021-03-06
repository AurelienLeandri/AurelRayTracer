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
// TODO: check for materials with multiple BxDFs if it works

glm::vec3 BSDF::f(const glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord &hit_record) const
{
    glm::vec3 w_i_local = hit_record.shadingCoordinateSystem * w_i;
    glm::vec3 w_o_local = hit_record.shadingCoordinateSystem * w_o;
    bool reflected = w_o_local.z * w_i_local.z > 0;  // Both w_i and w_o are on the same side of the surface -> Reflection. If not -> Transmission.
    glm::vec3 f(0, 0, 0);
    for (int i = 0; i < _bxdfs.size(); ++i) {
        if ((reflected && (_bxdfs[i]->type.isType(BxDF::Type::BXDF_REFLECTION))) || (!reflected && (_bxdfs[i]->type.isType(BxDF::Type::BXDF_TRANSMISSION)))) {
            f += _bxdfs[i]->f(w_i_local, w_o_local, hit_record);
        }
    }
    return f;
}

float BSDF::pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
    glm::vec3 w_i_local = hit_record.shadingCoordinateSystem * w_i;
    glm::vec3 w_o_local = hit_record.shadingCoordinateSystem * w_o;
    float pdf = 0;
    for (int i = 0; i < _bxdfs.size(); ++i) {
        pdf += _bxdfs[i]->pdf(w_i_local, w_o_local, hit_record);
    }
    return pdf / _bxdfs.size();
}

glm::vec3 BSDF::sample_f(glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord& hit_record, float &pdf, BxDF::Type &sampled_type, const BxDF::Type& bxdfTypes) const
{
    int nbComponents = nbMatchingComponents(bxdfTypes);
    if (!nbComponents) {
        pdf = 0;
        return glm::vec3(0);
    }

    glm::vec3 w_o_local = glm::normalize(hit_record.shadingCoordinateSystem * w_o);
    if (w_o.z == 0)
        return glm::vec3(0);

    int random_index = static_cast<int>(frand() * nbComponents);
    int bxdf_index = -1;
    do {
        bxdf_index++;
        if (bxdf_index >= nbComponents)
            return glm::vec3(0);
        if (_bxdfs[bxdf_index]->type.isType(bxdfTypes))
            random_index--;
    } while (random_index >= 0);

    glm::vec3 w_i_local(0);
    glm::vec3 f = _bxdfs[bxdf_index]->sample_f(w_i_local, w_o_local, hit_record, pdf);
    sampled_type = _bxdfs[bxdf_index]->type;

    if (pdf == 0 || f == glm::vec3(0, 0, 0))
        return glm::vec3(0);

    w_i = glm::transpose(hit_record.shadingCoordinateSystem) * w_i_local;

    for (int i = 0; i < _bxdfs.size(); ++i) {
        if (i == bxdf_index) continue;  // pdf already contains the value of the pdf we sampled from, plus its a nice way to get at least a value of 1 for pdf if we sampled using a specular BxDF
        pdf += _bxdfs[i]->pdf(w_i_local, w_o_local, hit_record);
    }
    pdf /= _bxdfs.size();

    if (pdf == 0)
        return glm::vec3(0);

    if (_bxdfs[bxdf_index]->type.isType(BxDF::Type::BXDF_SPECULAR) || _bxdfs.size() == 1)
        return f;

    bool reflected = w_i_local.z * w_o_local.z > 0;
    if (reflected == false)
        int a = 0;

    f = glm::vec3(0);
    for (int i = 0; i < _bxdfs.size(); ++i) {
        if ((reflected && _bxdfs[i]->type.isType(BxDF::Type::BXDF_REFLECTION)) || (!reflected && _bxdfs[i]->type.isType(BxDF::Type::BXDF_TRANSMISSION)))
            f += _bxdfs[i]->f(w_i_local, w_o_local, hit_record);
    }

    return f;
}

bool BSDF::add(std::shared_ptr<const BxDF> bxdf)
{
    if (_bxdfs.size() >= MAX_NB_BXDFS)
        return false;
    _bxdfs.push_back(bxdf);
    return true;
}

int BSDF::nbMatchingComponents(BxDF::Type type) const
{
    int numComponents = 0;
    for (std::shared_ptr<const BxDF> bxdf : _bxdfs) {
        if (bxdf->type.isType(type))
            numComponents++;
    }
    return numComponents;
}
