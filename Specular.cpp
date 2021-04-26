#include "Specular.h"

#include "HitRecord.h"
#include "Utils.h"

#include <iostream>

float fresnelDielectric(float cos_w_normal, float eta_i, float eta_t) {
	if (eta_t == 0)  // Special case for mirror TODO: remove once mirrors are handled physically correct
		return 1;

	float cos_i = glm::clamp(cos_w_normal, -1.f, 1.f);
	if (cos_i < 0) { // w_i comes from below the surface
		std::swap(eta_i, eta_t);
		cos_i = -cos_w_normal;
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

glm::vec3 fresnelConductor(float cos_w_normal, const glm::vec3& eta_i, const glm::vec3& eta_t, const glm::vec3& k) {
	cos_w_normal = glm::clamp(cos_w_normal, -1.f, 1.f);
	glm::vec3 eta = eta_t / eta_i;
	glm::vec3 eta_k = k / eta_i;

	float cos_w_normal2 = cos_w_normal * cos_w_normal;
	float sin2_w_normal = 1. - cos_w_normal2;
	glm::vec3 eta2 = eta * eta;
	glm::vec3 eta_k2 = eta_k * eta_k;

	glm::vec3 t0 = eta2 - eta_k2 - sin2_w_normal;
	glm::vec3 a2_plus_b2 = glm::sqrt(t0 * t0 + 4.f * eta2 * eta_k2);
	glm::vec3 t1 = a2_plus_b2 + cos_w_normal2;
	glm::vec3 a = glm::sqrt(0.5f * (a2_plus_b2 + t0));
	glm::vec3 t2 = (float)2 * cos_w_normal * a;
	glm::vec3 Rs = (t1 - t2) / (t1 + t2);

	glm::vec3 t3 = cos_w_normal2 * a2_plus_b2 + sin2_w_normal * sin2_w_normal;
	glm::vec3 t4 = t2 * sin2_w_normal;
	glm::vec3 Rp = Rs * (t3 - t4) / (t3 + t4);

	return 0.5f * (Rp + Rs);
}

SpecularTransmission::SpecularTransmission(float etaRay, float etaInterface, const glm::vec3& albedo, float k, bool fromLight)
	: BxDF(BxDF::BSDF_SPECULAR | BxDF::BSDF_TRANSMISSION), _etaRay(etaRay), _etaInterface(etaInterface), _albedo(albedo), _fromLight(fromLight)
{
}

glm::vec3 SpecularTransmission::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return glm::vec3(0);
}

float SpecularTransmission::pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return 0.0f;
}

glm::vec3 SpecularTransmission::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const
{
	float fresnel_reflectance = fresnelDielectric(w_o.z, _etaRay, _etaInterface);
	float eta_i_over_eta_t = 0;
	if (_etaInterface < _etaRay)
		eta_i_over_eta_t = _etaInterface / _etaRay;
	else
		eta_i_over_eta_t = _etaRay / _etaInterface;

	if (!refract(w_o, glm::vec3(0, 0, 1), eta_i_over_eta_t, w_i))
		return glm::vec3(0);

	glm::vec3 f = _albedo * (glm::vec3(1) - fresnel_reflectance) / glm::abs(w_i.z);
	if (_fromLight == true) {
		f *= eta_i_over_eta_t * eta_i_over_eta_t;
	}
	pdf = 1;
	hit_record.ray.eta = _etaInterface;
	return f;
}

SpecularReflection::SpecularReflection(float etaRay, float etaInterface, const glm::vec3& albedo, float k, bool fromLight)
	: BxDF(BxDF::BSDF_SPECULAR | BxDF::BSDF_REFLECTION), _etaRay(glm::vec3(etaRay)), _etaInterface(glm::vec3(etaInterface)), _albedo(albedo), _fromLight(fromLight), _k(glm::vec3(k))
{
}
SpecularReflection::SpecularReflection(const glm::vec3& etaRay, const glm::vec3& etaInterface, const glm::vec3& albedo, const glm::vec3& k, bool fromLight)
	: BxDF(BxDF::BSDF_SPECULAR | BxDF::BSDF_REFLECTION), _etaRay(etaRay), _etaInterface(etaInterface), _albedo(albedo), _fromLight(fromLight), _k(k)
{
}

glm::vec3 SpecularReflection::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const {
	return glm::vec3(0);
}

float SpecularReflection::pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return 0.0f;
}

glm::vec3 SpecularReflection::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const {
	glm::vec3 fresnel_reflectance(0);
	if (_k == glm::vec3(0)) {
		fresnel_reflectance = glm::vec3(fresnelDielectric(w_o.z, _etaRay.x, _etaInterface.x));
	}
	else {
		fresnel_reflectance = fresnelConductor(w_o.z, _etaRay, _etaInterface, _k);
	}
	w_i = glm::vec3(-w_o.x, -w_o.y, w_o.z);  // Reflect when the normal is (0, 0, 1)
	pdf = 1;
	return _albedo * glm::vec3(fresnel_reflectance) / glm::abs(w_i.z);
}

Specular::Specular(float etaRay, float etaInterface, const glm::vec3& albedo, float k, bool fromLight)
	: BxDF(BxDF::BSDF_SPECULAR | BxDF::BSDF_TRANSMISSION | BxDF::BSDF_REFLECTION), _etaRay(etaRay), _etaInterface(etaInterface), _albedo(albedo), _fromLight(fromLight)
{
}

glm::vec3 Specular::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return glm::vec3(0);
}

float Specular::pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	return 0;
}

glm::vec3 Specular::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const
{
	float fresnel_reflectance = fresnelDielectric(w_o.z, _etaRay, _etaInterface);
	if (frand() <= fresnel_reflectance) {  // Reflection
		w_i = glm::vec3(-w_o.x, -w_o.y, w_o.z);  // Reflect when the normal is (0, 0, 1)
		pdf = fresnel_reflectance;
		return _albedo * glm::vec3(fresnel_reflectance) / glm::abs(w_i.z);
	}
	else {  // Transmission
		float eta_i_over_eta_t = 0;
		if (_etaInterface < _etaRay)
			eta_i_over_eta_t = _etaInterface / _etaRay;
		else
			eta_i_over_eta_t = _etaRay / _etaInterface;

		if (!refract(w_o, glm::vec3(0, 0, 1), eta_i_over_eta_t, w_i))
			return glm::vec3(0);

		glm::vec3 f = _albedo * (glm::vec3(1) - fresnel_reflectance) / glm::abs(w_i.z);
		if (_fromLight == true) {
			f *= eta_i_over_eta_t * eta_i_over_eta_t;
		}
		pdf = 1 - fresnel_reflectance;
		hit_record.ray.eta = _etaInterface;
		return f;
	}
}
