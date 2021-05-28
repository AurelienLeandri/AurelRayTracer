#pragma once

#include "Distribution2D.h"

#include <glm/glm.hpp>

class InfiniteAreaLight
{
public:
	InfiniteAreaLight(const unsigned char* environment_texture, int width, int height, int _nbChannels);
	~InfiniteAreaLight();

public:
	float sample(glm::vec3& value, const glm::vec3& origin, const glm::vec3& origin_normal) const;

private:
	const unsigned char* _environmentTexture = nullptr;
	float* _luminanceMap = nullptr;
	const int _width;
	const int _height;
	const int _nbChannels;
	std::unique_ptr<Distribution2D> _distribution;

private:
	static const float _INV_255;

};

