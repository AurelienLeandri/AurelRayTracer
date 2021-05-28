#pragma once

#include <glm/glm.hpp>

struct HitRecord;

class MicrofacetDistribution
{
public:
	MicrofacetDistribution();

public:
	// Distribution of microfacet normals
	virtual float D(const glm::vec3& w_h) const = 0;
	// Masking-shadowing distribution function
	float G1(const glm::vec3& w) const;

	// Probability than a facet is visible from both wo and wi.
	float G(const glm::vec3& wo, const glm::vec3& wi) const;

	// Invisible masked microfacet area per visible microfacet area (A- / (1+ - A-)). Used to compute the masking function G
	// Models have their own lambda function. It is unique for a given D(w_h) if we assume that there is no local relationship between microfacets (untrue
	// in reality, but not impactful). I understood it as lambda does not vary wrt dA because D doesnt either.
	virtual float lambda(const glm::vec3& w) const = 0;

	virtual void sample_wh(glm::vec3& w_h) const = 0;

public:
	static float roughnessToAlpha(float roughness);
};

