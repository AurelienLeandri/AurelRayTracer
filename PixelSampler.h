#pragma once
#include "Sampler.h"

class PixelSampler :
    public Sampler
{
public:
    PixelSampler(size_t samplesPerPixel, size_t nSampledDimensions);

public:
    // Inherited via Sampler
    virtual float get1D() override;
    virtual glm::vec2 get2D() override;
    virtual bool startNextSample() override;

protected:
    std::vector<std::vector<float>> _samples1D;
    std::vector<std::vector<glm::vec2>> _samples2D;
    size_t _current1DDimension = 0, _current2DDimension = 0;
};

