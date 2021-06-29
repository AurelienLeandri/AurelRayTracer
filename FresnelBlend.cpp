#include "FresnelBlend.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "MicrofacetDistribution.h"
#include "Utils.h"

namespace {
	glm::vec3 fresnel_schlick(float cos_w_normal, const glm::vec3& specular_reflection) {
		return specular_reflection + (float)glm::pow(1 - cos_w_normal, 5) * (glm::vec3(1) - specular_reflection);
	}
}

FresnelBlend::FresnelBlend(const glm::vec3& fresnel_reflection, const glm::vec3& diffuse_reflection, std::shared_ptr<MicrofacetDistribution> reflection_model)
	: BxDF(BxDF::Type::BXDF_REFLECTION | BxDF::Type::BXDF_GLOSSY), _fresnelReflection(fresnel_reflection), _diffuseReflection(diffuse_reflection), _reflectionModel(reflection_model)
{
}

glm::vec3 FresnelBlend::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	glm::vec3 diffuse = (28.f / (23.f * (float)M_PI)) * _diffuseReflection *
		(glm::vec3(1.f) - _fresnelReflection) *
		(1 - (float)glm::pow(1 - .5f * glm::abs(w_i.z), 5)) *
		(1 - (float)glm::pow(1 - .5f * glm::abs(w_o.z), 5));
	glm::vec3 w_h = w_i + w_o;
	if (w_h.x == 0 && w_h.y == 0 && w_h.z == 0) return glm::vec3(0);
	w_h = glm::normalize(w_h);
	glm::vec3 specular = _reflectionModel->D(w_h) /
		(4 * glm::abs(glm::dot(w_i, w_h)) *
			std::max(glm::abs(w_i.z), glm::abs(w_o.z))) *
		fresnel_schlick(glm::dot(w_i, w_h), _fresnelReflection);
	return diffuse + specular;
}

float FresnelBlend::pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	glm::vec3 w_h = w_i + w_o;

	if (w_h.x == 0 && w_h.y == 0 && w_h.z == 0)
		return 0;

	w_h = glm::normalize(w_h);

	float D = _reflectionModel->D(w_h);
	float pdf_spec = D * glm::abs(w_h.z) / (4 * glm::dot(w_h, w_o));  // This is the pdf we used wrt to solid angle (hence the cos term).
	float pdf_diffuse = glm::abs(w_i.z) / (float)M_PI;
	return 0.5f * (pdf_spec + pdf_diffuse);
}

glm::vec3 FresnelBlend::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const
{
	if (w_o.z == 0)
		return glm::vec3(0);

	if (frand() < 0.5f) {  // Specular

		glm::vec3 w_h(0, 0, 0);

		_reflectionModel->sample_wh(w_h);
		float dot_w_o_w_h = glm::dot(w_o, w_h);
		if (dot_w_o_w_h < 0)
			return glm::vec3(0, 0, 0);
		while (dot_w_o_w_h < 0) {
			_reflectionModel->sample_wh(w_h);
			dot_w_o_w_h = glm::dot(w_o, w_h);
		}

		w_i = glm::normalize(reflect(w_o, w_h));

		if (w_o.z * w_i.z < 0)  // Has to be in the same hemisphere
			return glm::vec3(0);
	}
	else {
		do {
			w_i = glm::normalize(glm::vec3(0, 0, 1) + glm::normalize(random_in_unit_sphere()));
		} while (w_i.z <= 0.000001f);
	}

	glm::vec3 w_h = w_o + w_i;

	if (w_i.z == 0 || w_o.z == 0 || w_h == glm::vec3(0))
		return glm::vec3(0);

	w_h = glm::normalize(w_h);

	pdf = this->pdf(w_i, w_o, hit_record);
	return f(w_i, w_o, hit_record);
}
