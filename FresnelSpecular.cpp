#include "FresnelSpecular.h"

#include "HitRecord.h"
#include "Utils.h"

#include <iostream>

FresnelSpecular::FresnelSpecular(float etaRay, float etaInterface, const glm::vec3& albedo, bool fromLight)
	: BxDF(BxDF::BSDF_SPECULAR | BxDF::BSDF_REFLECTION | BxDF::BSDF_TRANSMISSION), _etaRay(etaRay), _etaInterface(etaInterface), _albedo(albedo), _fromLight(fromLight)
{
}

glm::vec3 FresnelSpecular::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return glm::vec3(0, 0, 0);
}

namespace {

	// NOTE: w_i for "incident", not "in" like in the rest of the project
	float fresnel_dielectric(const glm::vec3 w_i, float eta_i, float eta_t) {
		float cos_i = glm::clamp(w_i.z, -1.f, 1.f);
		if (cos_i < 0) { // w_i comes from below the surface
			std::swap(eta_i, eta_t);
			cos_i = -w_i.z;
		}
		
		// sin2 = 1 - cos2
		float sin_i = glm::sqrt(glm::max(0.f, 1 - cos_i * cos_i));
		
		// Snell's law for finding sin_t
		float sin_t = (eta_i / eta_t) * sin_i;
		if (sin_t >= 1)  // Total internal reflection
			return 1;

		// cos2 = 1 - sin2
		float cos_t = glm::sqrt(glm::max(0.f, 1 - sin_t * sin_t));

		// Fresnel formula for dielectrics
		float r_parallel = (eta_t * cos_i - eta_i * cos_t) / (eta_t * cos_i + eta_i * cos_t);
		float r_perpendicular = (eta_i * cos_i - eta_t * cos_t) / (eta_i * cos_i + eta_t * cos_t);
		return (r_parallel * r_parallel + r_perpendicular * r_perpendicular) * 0.5f;
	}

	float fresnel_conductor(const glm::vec3 w_i, float eta_i, float eta_t, float k) {
		return 0;
	}
}

glm::vec3 FresnelSpecular::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float &pdf) const
{
	if (_etaInterface < 1) {  // Fully reflective material
		w_i = glm::vec3(-w_o.x, -w_o.y, w_o.z);
		pdf = 1;
		float inv_cos_theta = 1 / glm::abs(w_i.z);
		return _albedo * glm::vec3(inv_cos_theta);
	}
	float fresnel_reflectance = _k == 0 ? fresnel_dielectric(w_o, _etaRay, _etaInterface) : fresnel_conductor(w_o, _etaRay, _etaInterface, 0);
	if (frand() < fresnel_reflectance) {  // Reflection
		w_i = glm::vec3(-w_o.x, -w_o.y, w_o.z);
		float cos_w_i = w_i.z;
		pdf = fresnel_reflectance;
		return _albedo * glm::vec3(fresnel_reflectance) / glm::abs(cos_w_i);
	}
	else {  // Transmition
		float eta_i_over_eta_t = 0;
		if (_etaInterface < _etaRay)
			eta_i_over_eta_t = _etaInterface / _etaRay;
		else
			eta_i_over_eta_t = _etaRay / _etaInterface;

		if (!refract(w_o, glm::vec3(0, 0, 1), eta_i_over_eta_t, w_i))
			return glm::vec3(0, 0, 0);

		glm::vec3 f = _albedo * (glm::vec3(1, 1, 1) - fresnel_reflectance) / glm::abs(w_i.z);
		if (_fromLight == true) {
			f *= eta_i_over_eta_t * eta_i_over_eta_t;
		}
		pdf = 1 - fresnel_reflectance;
		hit_record.ray.eta = _etaInterface;
		return f;
	}
}
