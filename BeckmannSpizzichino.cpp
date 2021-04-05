#include "BeckmannSpizzichino.h"

#define _USE_MATH_DEFINES
#include <math.h>

BeckmannSpizzichino::BeckmannSpizzichino(float alpha) : _alphaX(alpha), _alphaY(alpha)
{
}

BeckmannSpizzichino::BeckmannSpizzichino(float alpha_x, float alpha_y) : _alphaX(alpha_x), _alphaY(alpha_y)
{
}

float BeckmannSpizzichino::D(const glm::vec3& w_h)
{
	float tan_theta_h = glm::tan(w_h.z);
	float tan2_theta_h = tan_theta_h * tan_theta_h;
	if (std::isinf(tan2_theta_h))  // Avoid infinite value for tan2(theta_h)
		return 0;
	float cos_phi_h = w_h.x * glm::sqrt(glm::max(0.f, 1 - w_h.x * w_h.z));
	float cos2_phi_h = cos_phi_h * cos_phi_h;
	float sin2_phi_h = glm::max(0.f, 1 - cos2_phi_h);
	return glm::exp(-tan2_theta_h * ((cos2_phi_h / (_alphaX * _alphaX)) + (sin2_phi_h / (_alphaY * _alphaY)))) / ((float)(M_PI) * _alphaX * _alphaY * (float)glm::pow(w_h.z, 4));
}
