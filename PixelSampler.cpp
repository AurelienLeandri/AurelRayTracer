#include "PixelSampler.h"

#include "Utils.h"

PixelSampler::PixelSampler(size_t samplesPerPixel, int nSampledDimensions)
    : Sampler(samplesPerPixel)
{
    for (int i = 0; i < nSampledDimensions; ++i) {
        _samples1D.push_back(std::vector<float>(samplesPerPixel));
        _samples2D.push_back(std::vector<glm::vec2>(samplesPerPixel));
    }
}

glm::vec2 PixelSampler::get2D()
{
    if (_current2DDimension < _samples2D.size())
        return _samples2D[_current2DDimension++][_currentPixelSampleIndex];
    return glm::vec2(frand(), frand());
}

bool PixelSampler::startNextSample()
{
    _current2DDimension = _current1DDimension = 0;
    return Sampler::startNextSample();
}

float PixelSampler::get1D() {
    if (_current1DDimension < _samples1D.size())
        return _samples1D[_current1DDimension++][_currentPixelSampleIndex];
    return frand();
}

