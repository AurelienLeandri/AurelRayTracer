#include "InfiniteAreaLight.h"

#include "Utils.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <fstream>

const float InfiniteAreaLight::_INV_255 = 1.f / 255;

InfiniteAreaLight::InfiniteAreaLight(const unsigned char* environment_texture, int width, int height, int nb_channels) :
	_environmentTexture(environment_texture), _width(width), _height(height), _nbChannels(nb_channels)
{
	_luminanceMap = new float[size_t(_width) * _height];
	for (int i = 0; i < _width * _height; i++)
		_luminanceMap[i] = 0;

	for (int i = 0; i < width * height * _nbChannels; i += _nbChannels) {
		float e = environment_texture[i] * 0.3f + environment_texture[i + 1] * 0.59f + environment_texture[i + 2] * 0.11f;
		if (true)
			int a = 0;
		_luminanceMap[i / _nbChannels] = e * _INV_255;
	}

	for (int i = 0; i < width * height; i++) {
		if (_luminanceMap[i] < 0)
			int a = 0;
	}

	for (int i = 0; i < height; i++) {
		float sin_theta = std::sin(float(M_PI) * (i + 0.5f) / float(height));
		for (int j = 0; j < width; j++) {
			_luminanceMap[i * width + j] *= sin_theta;
		}
	}

	_distribution = std::make_unique<Distribution2D>(_luminanceMap, height, width);
}

InfiniteAreaLight::~InfiniteAreaLight()
{
	if (_luminanceMap) {
		delete[] _luminanceMap;
		_luminanceMap = nullptr;
	}
}

float InfiniteAreaLight::sample(glm::vec3& value, const glm::vec3& origin, const glm::vec3& origin_normal) const
{
	if (_distribution) {
		int i = -1, j = -1;
		// TODO: give u and v between 0, 1 instead of indices.
		float pdf = _distribution->sample(frand(), frand(), i, j);

		if (pdf == 0)
			return 0;

		float theta = float(M_PI) * float(i) / _height;
		float sin_theta = glm::sin(theta);

		if (sin_theta == 0)
			return 0;

		float phi = 2 * float(M_PI) * float(j) / _width;
		float sin_phi = glm::sin(phi);
		float cos_phi = glm::cos(phi);
		value = glm::vec3(sin_theta * cos_phi, sin_theta * sin_phi, sin_theta);
		pdf = pdf / (2 * float(M_PI) * float(M_PI) * sin_theta);  // map p(u, v) to p(w) ("value") which is a solid angle.
		return pdf;
	}

	return 0;
}
