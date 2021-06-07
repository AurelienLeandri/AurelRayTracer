#include "Sampler.h"

#include "Camera.h"

Sampler::Sampler(size_t samplesPerPixel)
    : _samplesPerPixel(samplesPerPixel)
{
}

void Sampler::startPixel(const glm::ivec2& pixel)
{
    _currentPixel = pixel;
    _currentPixelSampleIndex = 0;
    array1DOffset = array2DOffset = 0;
}

void Sampler::request1DArray(int n)
{
    _samples1DArraySizes.push_back(n);
    _sampleArray1D.push_back(std::vector<float>(n * _samplesPerPixel)); 
}

void Sampler::request2DArray(int n)
{
    _samples2DArraySizes.push_back(n);
    _sampleArray2D.push_back(std::vector<glm::vec2>(n * _samplesPerPixel));
}

const float* Sampler::get1DArray(int n)
{
    if (array1DOffset == _sampleArray1D.size())
        return nullptr;
    return &_sampleArray1D[array1DOffset++][_currentPixelSampleIndex * n];
}

const glm::vec2* Sampler::get2DArray(int n)
{
    if (array2DOffset == _sampleArray2D.size())
        return nullptr;
    return &_sampleArray2D[array2DOffset++][_currentPixelSampleIndex * n];
}

CameraSample Sampler::generateCameraSample(const glm::ivec2& pixel)
{
    CameraSample cs;
    cs.pFilm = (glm::vec2)pixel + get2D();
    cs.time = get1D();
    cs.pLens = get2D();
    return cs;
}

int Sampler::getSamplesPerPixel() const
{
    return _samplesPerPixel;
}

bool Sampler::startNextSample()
{
    return ++_currentPixelSampleIndex < _samplesPerPixel;
}
