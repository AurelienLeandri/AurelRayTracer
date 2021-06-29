#include "LambertianTransmission.h"

LambertianTransmission::LambertianTransmission(glm::vec3 albedo, float refractionIndex, std::shared_ptr<Texture> albedoTexture)
	: BxDF(BxDF::Type::BXDF_TRANSMISSION | BxDF::Type::BXDF_DIFFUSE), _albedoValue(albedo), _refractionIndex(refractionIndex), _albedoTexture(albedoTexture)
{
}

LambertianTransmission::~LambertianTransmission()
{
}

glm::vec3 LambertianTransmission::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return glm::vec3();
}

float LambertianTransmission::pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return 0.0f;
}

glm::vec3 LambertianTransmission::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return glm::vec3();
}
