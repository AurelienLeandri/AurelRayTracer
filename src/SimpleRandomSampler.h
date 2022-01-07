#pragma once
#include "PixelSampler.h"

class SimpleRandomSampler :
    public PixelSampler
{
public:
	SimpleRandomSampler(size_t xPixelSamples, size_t yPixelSamples, int nSampledDimensions);

public:
	// Inherited via Sampler
	virtual Sampler* clone(int seed) const override;
	virtual void startPixel(const glm::ivec2& pixel) override;

private:
	const size_t _xPixelSamples, _yPixelSamples;
};

