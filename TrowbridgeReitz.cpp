#include "TrowbridgeReitz.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Utils.h"

#include <iostream>

TrowbridgeReitz::TrowbridgeReitz(float alpha) : _alphaX(alpha), _alphaY(alpha)
{
}

TrowbridgeReitz::TrowbridgeReitz(float alpha_x, float alpha_y) : _alphaX(alpha_x), _alphaY(alpha_y)
{
}

float TrowbridgeReitz::D(const glm::vec3& w_h) const
{
	float cos2_theta_h = w_h.z * w_h.z;
	/*
	float sin2_theta_h = glm::max(0.f, 1 - cos2_theta_h);
	float tan2_theta_h = sin2_theta_h / cos2_theta_h;
	if (std::isinf(tan2_theta_h))  // Avoid infinite value for tan2(theta_h)
		return 0;
	float cos4_theta_h = cos2_theta_h * cos2_theta_h;
	float sin_theta_h = glm::sqrt(sin2_theta_h);
	float cos_phi_h = sin_theta_h == 0 ? 0 : glm::clamp(w_h.x / sin_theta_h, (float)-1, (float)1);
	float cos2_phi_h = cos_phi_h * cos_phi_h;
	float sin2_phi_h = glm::max(0.f, 1 - cos2_phi_h);
	float alphax2 = _alphaX * _alphaX;
	float alphay2 = _alphaY * _alphaY;
	*/
	/*
	float e = tan2_theta_h * ((cos2_phi_h / (_alphaX * _alphaX)) + (sin2_phi_h / (_alphaY * _alphaY)));
	return 1 / ((float)M_PI * _alphaX * _alphaY * cos4_theta_h * (1 + e) * (1 + e));
	*/
	return _alphaX * _alphaX / ((float)M_PI * (float)glm::pow((_alphaX * _alphaX - 1) * cos2_theta_h + 1, 2));
}

float TrowbridgeReitz::lambda(const glm::vec3& w) const
{
	// Direct transcription of the lambda function for TR
	float sin_theta_h = glm::sqrt(glm::max(0.f, 1 - w.z * w.z));
	float abs_tan_theta_h = glm::abs(sin_theta_h / w.z);
	if (glm::isinf(abs_tan_theta_h)) return 0.f;

	float cos_phi_h = sin_theta_h == 0 ? 0 : glm::clamp(w.x / sin_theta_h, -1.f, 1.f);
	float cos2_phi_h = cos_phi_h * cos_phi_h;
	float sin2_phi_h = glm::max(0.f, 1 - cos2_phi_h);

	float alpha = glm::sqrt(cos2_phi_h * _alphaX * _alphaX + sin2_phi_h * _alphaY * _alphaY);

	return (glm::sqrt(1 + (float)glm::pow(alpha * abs_tan_theta_h, 2)) - 1) / 2;
}

void TrowbridgeReitz::sample_wh(glm::vec3& w_h) const
{
	float e_theta = frand();
	float e_phi = frand();
	float theta = glm::acos(glm::sqrt((glm::max(0.f, 1 - e_theta)) / (e_theta * (_alphaX * _alphaX - 1) + 1)));
	float phi = e_phi * 2 * (float)M_PI;
	float cos_theta = glm::cos(theta);
	float sin_theta = glm::sin(theta);
	float cos_phi = glm::cos(phi);
	float sin_phi = glm::sin(phi);
	w_h = glm::vec3(cos_phi * sin_theta, sin_phi * sin_theta, cos_theta);
	if (w_h.z < 0)
		w_h = -w_h;
	w_h = glm::normalize(w_h);
}
