#pragma once
#include "PixelSampler.h"

class StratifiedSampler : public PixelSampler
{
public:
	StratifiedSampler(size_t xPixelSamples, size_t yPixelSamples, int nSampledDimensions);

public:
	// Inherited via Sampler
	virtual Sampler* clone(int seed) const override;
	virtual void startPixel(const glm::ivec2& pixel) override;

private:
	void _latinHypercubeSampling2D(glm::vec2 *samplesArray, int nbSamples);

private:
	const size_t _xPixelSamples, _yPixelSamples;
};

