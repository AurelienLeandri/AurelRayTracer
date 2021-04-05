#pragma once

#include <glm.hpp>

class MicrofacetDistribution
{
public:
	MicrofacetDistribution();

public:
	virtual float D(const glm::vec3& w_h) = 0;

public:
	static float roughnessToAlpha(float roughness);
};

