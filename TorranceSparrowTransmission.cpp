#include "TorranceSparrowTransmission.h"

#include "MicrofacetDistribution.h"

#include "FresnelSpecular.h"
#include "HitRecord.h"
#include "Utils.h"

TorranceSparrowTransmission::TorranceSparrowTransmission(float etaRay, float etaInterface, const glm::vec3& albedo, std::shared_ptr<MicrofacetDistribution> reflection_model)
	: BxDF(BxDF::BSDF_TRANSMISSION | BxDF::BSDF_GLOSSY), _etaRay(etaRay), _etaInterface(etaInterface), _albedo(albedo), _reflectionModel(reflection_model)
{
}

glm::vec3 TorranceSparrowTransmission::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	float eta_i_over_eta_t = w_o.z > 0 ? _etaRay / _etaInterface : _etaRay / _etaInterface;
	if (glm::abs(eta_i_over_eta_t - 1.f) < 1e-4)  // Case both etas are ~=, to avoid division by zero for the f value
		return _albedo;

	float cos_w_o = w_o.z;
	float cos_w_i = w_i.z;
	glm::vec3 w_h = glm::normalize(w_o + _etaInterface * w_i);

	if (cos_w_i == 0 || cos_w_o == 0 || w_h == glm::vec3(0, 0, 0))
		return glm::vec3(0, 0, 0);

	if (w_h.z < 0)  // Happens often since w_i is the transmission.
		w_h = -w_h;

	float dot_w_o_w_h = glm::dot(w_o, w_h);
	float dot_w_i_w_h = glm::dot(w_i, w_h);

	float D_w_h = _reflectionModel->D(w_h);
	float G_w_o_w_i = _reflectionModel->G(w_o, w_i);
	float cos_w_o_w_h = glm::abs(glm::dot(w_o, w_h));
	float fresnel_w_o = FresnelSpecular::fresnelDielectric(cos_w_o_w_h, _etaRay, _etaInterface);

	return _albedo * glm::abs(D_w_h * G_w_o_w_i * (1 - fresnel_w_o) * glm::abs(dot_w_i_w_h) * glm::abs(dot_w_o_w_h) / ((float)glm::pow(dot_w_o_w_h + eta_i_over_eta_t * dot_w_i_w_h, 2) * cos_w_o * cos_w_i));
}

glm::vec3 TorranceSparrowTransmission::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const
{
	if (w_o.z == 0)
		return glm::vec3(0, 0, 0);

	glm::vec3 w_h(0, 0, 0);
	_reflectionModel->sample_wh(w_h);
	if (glm::dot(w_o, w_h) < 0)
		return glm::vec3(0, 0, 0);

	float eta_i_over_eta_t = w_o.z > 0 ? _etaRay / _etaInterface : _etaInterface / _etaRay;
	if (!refract(w_o, w_h, eta_i_over_eta_t, w_i))
		return glm::vec3(0, 0, 0);

	if (w_o.z * w_i.z > 0)
		return glm::vec3(0, 0, 0);

	pdf = _reflectionModel->D(w_h) * glm::abs(w_i.z);  // This is the pdf we used wrt to solid angle (hence the cos term).
	return f(w_i, w_o, hit_record);
}
