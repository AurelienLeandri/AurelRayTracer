#include "LambertianReflection.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Utils.h"
#include "HitRecord.h"
#include "Texture.h"

LambertianReflection::LambertianReflection(glm::vec3 albedo, std::shared_ptr<Texture> albedoTexture) :
    BxDF(BxDF::Type::BSDF_REFLECTION | BxDF::Type::BSDF_DIFFUSE),
    _albedoValue(albedo),
    _albedoTexture(albedoTexture)
{
}


LambertianReflection::~LambertianReflection()
{
}

glm::vec3 LambertianReflection::f(const glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord& hit_record) const
{
    glm::vec3 albedo = _albedoTexture ? _albedoTexture->color(hit_record.u, hit_record.v, hit_record.position) : _albedoValue;
    return albedo / float(M_PI);
}

glm::vec3 LambertianReflection::sample_f(glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord& hit_record) const
{
    do {
        w_i = hit_record.normal + glm::normalize(random_in_unit_sphere());
    } while (glm::dot(hit_record.normal, w_i) <= 0.000001f);
    w_i = glm::normalize(w_i);
    return glm::vec3(glm::dot(hit_record.normal, w_i)) / float(M_PI);
}
