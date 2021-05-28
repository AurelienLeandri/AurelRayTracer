#include "OrenNayarReflection.h"

#include <glm/glm.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#include "HitRecord.h"
#include "Utils.h"

OrenNayarReflection::OrenNayarReflection(const glm::vec3& albedo, float sigma)
	: BxDF(BxDF::Type::BSDF_REFLECTION | BxDF::Type::BSDF_DIFFUSE), _albedo(albedo)
{
	float sigma2 = sigma * sigma;
	_A = 1 - (sigma2 / (2 * (sigma2 + 0.33f)));
	_B = (0.45f * sigma2) / (sigma2 + 0.09f);
}

glm::vec3 OrenNayarReflection::f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
	// for computing sin(alpha) and tan(beta) we need both sins
	float sin_theta_i = glm::sqrt(glm::max(0.f, 1 - (w_i.z * w_i.z)));
	float sin_theta_o = glm::sqrt(glm::max(0.f, 1 - (w_o.z * w_o.z)));

	// cos(phi_i - phi_o) = cos(phi_i) * cos(phi_o) + sin(phi_i) * sin(phi_o). Let's avoid arcsin(theta).
	float cos_phi_i = w_i.x / sin_theta_i;  // cos(phi) = x / r where r the radius of the base disk, which is sin(theta).
	float sin_phi_i = w_i.y / sin_theta_i;
	float cos_phi_o = w_o.x / sin_theta_o;
	float sin_phi_o = w_o.y / sin_theta_o;
	float cos_phi_i_minus_phi_o = cos_phi_i * cos_phi_o + sin_phi_i * sin_phi_o;

	// TODO: avoid infinite values for tan_beta below
	float sin_alpha_tan_beta = 0;
	if (w_i.z < w_o.z) {
		sin_alpha_tan_beta = sin_theta_i * (sin_theta_o / w_o.z);
	}
	else {
		sin_alpha_tan_beta = sin_theta_o * (sin_theta_i / w_i.z);
	}

	// Plus jamais je fais ça mdeir.
	return (_albedo / (float)M_PI) * (_A + _B * glm::max(0.f, cos_phi_i_minus_phi_o) * sin_alpha_tan_beta);
}

