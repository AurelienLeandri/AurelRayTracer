#include "OrenNayarReflection.h"

#include "HitRecord.h"

OrenNayarReflection::OrenNayarReflection(const glm::vec3& albedo, float sigma)
	: BxDF(BxDF::Type::BSDF_REFLECTION | BxDF::Type::BSDF_DIFFUSE), _albedo(albedo)
{
	float sigma2 = sigma * sigma;
	_A = 1 - (sigma2 / (2 * (sigma2 + 0.33f)));
	_B = (0.45f * sigma2) / (sigma2 + 0.09f);
}

glm::vec3 OrenNayarReflection::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return glm::vec3();
}

glm::vec3 OrenNayarReflection::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const
{
	return glm::vec3();
}
