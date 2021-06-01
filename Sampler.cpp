#include "Sampler.h"

Sampler* Sampler::clone(int seed) const
{
    return new Sampler(*this);
}

void Sampler::startPixel(const glm::ivec2& pixel)
{
}

CameraSample Sampler::generateCameraSample(const glm::ivec2& pixel)
{
    return CameraSample();
}

int Sampler::getSamplesPerPixel() const
{
    return _samplesPerPixel;
}

bool Sampler::startNextSample()
{
    return false;
}
