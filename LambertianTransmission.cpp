#include "LambertianTransmission.h"

LambertianTransmission::LambertianTransmission(glm::vec3 albedo, float refractionIndex, std::shared_ptr<Texture> albedoTexture)
	: BxDF(BxDF::BSDF_TRANSMISSION | BxDF::BSDF_DIFFUSE), _albedoValue(albedo), _refractionIndex(refractionIndex), _albedoTexture(albedoTexture)
{
}

LambertianTransmission::~LambertianTransmission()
{
}

glm::vec3 LambertianTransmission::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return glm::vec3();
}

glm::vec3 LambertianTransmission::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return glm::vec3();
}
