#include "BeckmannSpizzichino.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Utils.h"

BeckmannSpizzichino::BeckmannSpizzichino(float alpha) : _alphaX(alpha), _alphaY(alpha)
{
}

BeckmannSpizzichino::BeckmannSpizzichino(float alpha_x, float alpha_y) : _alphaX(alpha_x), _alphaY(alpha_y)
{
}

float BeckmannSpizzichino::D(const glm::vec3& w_h) const
{
	if (_alphaX == 0 || _alphaY == 0) {
		return (w_h.x == 0 && w_h.y == 0 ? 1.f : 0.f);
	}
	float sin2_theta_h = glm::max(0.f, 1 - w_h.z * w_h.z);
	float tan2_theta_h = sin2_theta_h / (w_h.z * w_h.z);
	if (std::isinf(tan2_theta_h))  // Avoid infinite value for tan2(theta_h)
		return 0;
	float cos_phi_h = sin2_theta_h == 0 ? 0 : glm::clamp(w_h.x / glm::sqrt(sin2_theta_h), -1.f, 1.f);
	float cos2_phi_h = cos_phi_h * cos_phi_h;
	float sin2_phi_h = glm::max(0.f, 1 - cos2_phi_h);
	return glm::exp(-tan2_theta_h * ((cos2_phi_h / (_alphaX * _alphaX)) + (sin2_phi_h / (_alphaY * _alphaY)))) / ((float)(M_PI) * _alphaX * _alphaY * (float)glm::pow(w_h.z, 4));
}

float BeckmannSpizzichino::lambda(const glm::vec3& w) const
{
	// Instead of using the original function, we use an approximation taken from pbrt. Takes less time to compute.
	float sin_theta_h = glm::sqrt(glm::max(0.f, 1 - w.z * w.z));
	float abs_tan_theta_h = glm::abs(sin_theta_h / w.z);
	if (std::isinf(abs_tan_theta_h))  // Avoid infinite value for tan2(theta_h)
		return 0;

	float cos_phi_h = w.x * glm::sqrt(glm::max(0.f, 1 - w.x * w.z));
	float cos2_phi_h = cos_phi_h * cos_phi_h;
	float sin2_phi_h = glm::max(0.f, 1 - cos2_phi_h);

	float alpha = glm::sqrt(cos2_phi_h * _alphaX * _alphaX + sin2_phi_h * _alphaY * _alphaY);

	float a = 1 / (alpha * abs_tan_theta_h);
	if (a >= 1.6f)
		return 0;
	return (1 - 1.259f * a + 0.396f * a * a) /
		(3.535f * a + 2.181f * a * a);
}

void BeckmannSpizzichino::sample_wh(glm::vec3& w_h) const
{
	float e_theta = frand();
	float e_phi = frand();
	float theta = glm::acos(glm::sqrt(1 / (1 - _alphaX * _alphaX * glm::log(glm::max(0.f, 1 - e_theta)))));
	float phi = e_phi * 2 * (float)M_PI;
	float cos_theta = glm::clamp(glm::cos(theta), 0.f, 1.f);
	float sin_theta = glm::clamp(glm::sin(theta), 0.f, 1.f);
	float cos_phi = glm::clamp(glm::cos(phi), 0.f, 1.f);
	float sin_phi = glm::clamp(glm::sin(phi), 0.f, 1.f);
	w_h = glm::vec3(cos_phi * sin_theta, sin_phi * sin_theta, cos_theta);
	if (w_h.z < 0)
		w_h = -w_h;
}
