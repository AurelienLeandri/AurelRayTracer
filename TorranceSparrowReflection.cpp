#include "TorranceSparrowReflection.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "MicrofacetDistribution.h"

#include "Specular.h"
#include "HitRecord.h"

#include "Utils.h"

#include <iostream>


TorranceSparrowReflection::TorranceSparrowReflection(float etaRay, float etaInterface, const glm::vec3& albedo, std::shared_ptr<MicrofacetDistribution> reflection_model, float k)
	: BxDF(BxDF::BSDF_REFLECTION | BxDF::BSDF_GLOSSY), _etaRay(etaRay), _etaInterface(etaInterface), _albedo(albedo), _reflectionModel(reflection_model), _k(k)
{
}

glm::vec3 TorranceSparrowReflection::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	float cos_w_o = w_o.z;
	float cos_w_i = w_i.z;
	glm::vec3 w_h = w_i + w_o;

	if (cos_w_i == 0 || cos_w_o == 0 || w_h == glm::vec3(0))
		return glm::vec3(0);

	w_h = glm::normalize(w_h);
	
	float D_w_h = _reflectionModel->D(w_h);
	float G_w_o_w_i = _reflectionModel->G(w_o, w_i);
	float cos_w_i_w_h = glm::dot(w_i, w_h);
	//float fresnel_w_o = (_k != 0 ? fresnelConductor(cos_w_i_w_h, _etaRay, _etaInterface, _k) : fresnelDielectric(cos_w_i_w_h, _etaRay, _etaInterface));
	float fresnel_w_o = fresnelDielectric(cos_w_i_w_h, _etaRay, _etaInterface);
	glm::vec3 f = _albedo * D_w_h * G_w_o_w_i * fresnel_w_o / (4 * cos_w_i * cos_w_o);
	if (f.x > 1 || f.y > 1 || f.z > 1) {
		int a = 0;
		a = a;
	}
	return f;
}

glm::vec3 TorranceSparrowReflection::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const
{
	if (w_o.z == 0)
		return glm::vec3(0);

	glm::vec3 w_h(0, 0, 0);
	_reflectionModel->sample_wh(w_h);
	w_h = glm::normalize(w_h);
	auto bw_o = glm::normalize(w_o);
	if (glm::dot(w_o, w_h) < 0)  // w�o has to point toward the surface of the microfacet
		return glm::vec3(0);

	w_i = glm::normalize(reflect(w_o, w_h));

	if (w_o.z * w_i.z < 0)  // Has to be in the same hemisphere
		return glm::vec3(0);

	// Its somewhere in this im sure.
	// Maybe w_i.z instead of w_o, plus some wheights. I need to look that up
	float D = _reflectionModel->D(w_h);
	pdf = D * glm::abs(w_h.z) / (4 * glm::dot(w_o, w_h));  // This is the pdf we used wrt to solid angle (hence the cos term).
	if (pdf > 1) {
		return glm::vec3(0);
	}
	return f(w_i, w_o, hit_record);
}