#include "TorranceSparrowTransmission.h"

#include "MicrofacetDistribution.h"

#include "Specular.h"
#include "HitRecord.h"
#include "Utils.h"

#include <iostream>

TorranceSparrowTransmission::TorranceSparrowTransmission(float etaRay, float etaInterface, const glm::vec3& albedo, std::shared_ptr<MicrofacetDistribution> reflection_model)
	: BxDF(BxDF::Type::BXDF_TRANSMISSION | BxDF::Type::BXDF_GLOSSY), _etaRay(etaRay), _etaInterface(etaInterface), albedo(albedo), _reflectionModel(reflection_model)
{
}

glm::vec3 TorranceSparrowTransmission::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	if (w_o.z * w_i.z > 0)  // Transmission only
		return glm::vec3(0);

	float eta = w_o.z > 0 ? _etaInterface / _etaRay : _etaRay / _etaInterface;
	if (glm::abs(eta - 1.f) < 1e-4f)  // Case both etas are ~=, to avoid division by zero for the f value
		return albedo;

	float cos_w_o = w_o.z;
	float cos_w_i = w_i.z;
	glm::vec3 w_h = w_o + eta * w_i;

	if (cos_w_i == 0 || cos_w_o == 0 || w_h == glm::vec3(0))
		return glm::vec3(0);

	w_h = glm::normalize(w_h);

	if (w_h.z < 0)  // Happens often since w_i is the transmission.
		w_h = -w_h;

	float dot_w_o_w_h = glm::dot(w_o, w_h);
	float dot_w_i_w_h = glm::dot(w_i, w_h);

	// *At microsurface level*, are we on the same side of the facet? (same test as w_o.z * w_i.z)
	if (dot_w_o_w_h * dot_w_i_w_h > 0)
		return glm::vec3(0);

	float D_w_h = _reflectionModel->D(w_h);
	float G_w_o_w_i = _reflectionModel->G(w_o, w_i);
	float fresnel_w_o = fresnelDielectric(dot_w_o_w_h, _etaRay, _etaInterface);
	// TODO: test "if (from_light)". (for sampling; there is an additional factor here, see pbrt)
	float denom = ((float)glm::pow(dot_w_o_w_h + eta * dot_w_i_w_h, 2) * cos_w_o * cos_w_i);
	glm::vec3 f = glm::abs(albedo * D_w_h * G_w_o_w_i * (1 - fresnel_w_o) * eta * eta * glm::abs(dot_w_i_w_h) * glm::abs(dot_w_o_w_h) / denom);

	return f;
}

float TorranceSparrowTransmission::pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	if (w_o.z * w_i.z > 0)  // Both w_i and w_o are in the same hemisphere. We handle only transmission so we return.
		return 0;

	float eta = w_o.z > 0 ? _etaInterface / _etaRay : _etaRay / _etaInterface;
	float cos_w_o = w_o.z;
	float cos_w_i = w_i.z;
	glm::vec3 w_h = w_o + eta * w_i;

	if (cos_w_i == 0 || cos_w_o == 0 || w_h == glm::vec3(0))
		return 0;

	w_h = glm::normalize(w_h);

	float dot_w_o_w_h = glm::dot(w_o, w_h);
	float dot_w_i_w_h = glm::dot(w_i, w_h);

	float dw_h_over_dw_i = eta * eta * glm::abs(dot_w_i_w_h) / (float)glm::pow(eta * dot_w_i_w_h + dot_w_o_w_h, 2);

	float D = _reflectionModel->D(w_h);
	return D * glm::abs(w_h.z) * dw_h_over_dw_i;  // This is the pdf we used wrt to solid angle (hence the cos term).
}

glm::vec3 TorranceSparrowTransmission::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const
{
	if (w_o.z == 0)
		return glm::vec3(0);

	glm::vec3 w_h(0, 0, 0);
	_reflectionModel->sample_wh(w_h);
	float dot_w_o_w_h = glm::dot(w_o, w_h);
	while (dot_w_o_w_h < 0) {
		_reflectionModel->sample_wh(w_h);
		dot_w_o_w_h = glm::dot(w_o, w_h);
	}

	float eta_i_over_eta_t = w_o.z > 0 ? _etaRay / _etaInterface : _etaInterface / _etaRay;
	if (glm::abs(eta_i_over_eta_t - 1.f) < 1e-4f) {  // Case both etas are ~=, to avoid division by zero for the pdf value
		w_i = -w_o;
		pdf = glm::abs(w_i.z);
		return f(w_i, w_o, hit_record);
	}

	if (!refract(w_o, w_h, eta_i_over_eta_t, w_i))
		return glm::vec3(0);

	if (w_o.z * w_i.z > 0)  // Both w_i and w_o are in the same hemisphere. We handle only transmission so we return.
		return glm::vec3(0);

	pdf = this->pdf(w_i, w_o, hit_record);  // This is the pdf we used wrt to solid angle (hence the cos term).
	hit_record.ray.eta = _etaInterface;
	glm::vec3 brdf = f(w_i, w_o, hit_record);
	return brdf;
}
