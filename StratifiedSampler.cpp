#include "StratifiedSampler.h"

#include "Utils.h"

#include <algorithm>
#include <random>

namespace {
	std::default_random_engine rngShuffle;
}

StratifiedSampler::StratifiedSampler(size_t xPixelSamples, size_t yPixelSamples, int nSampledDimensions)
	: PixelSampler(xPixelSamples * yPixelSamples, nSampledDimensions), _xPixelSamples(xPixelSamples), _yPixelSamples(yPixelSamples)
{
}

void StratifiedSampler::startPixel(const glm::ivec2& pixel)
{
	// Create 1D sample values
	float inv1DSamples = 1.f / (_samplesPerPixel);  // Because _samples per pixel is _xPixelSamples * _yPixelSamples.
	for (int dimension = 0; dimension < _samples1D.size(); ++dimension) {
		for (int sample = 0; sample < _samplesPerPixel; ++sample) {
			float delta = frand();
			_samples1D[dimension][sample] = (sample + delta) * inv1DSamples;
		}
		std::shuffle(_samples1D[dimension].begin(), _samples1D[dimension].end(), rngShuffle);
	}

	// Create 2D sample values
	float inv2DSamplesX = 1.f / _xPixelSamples;
	float inv2DSamplesY = 1.f / _yPixelSamples;
	for (int dimension = 0; dimension < _samples2D.size(); ++dimension) {
		int iSample2D = 0;
		for (int sampleX = 0; sampleX < _xPixelSamples; ++sampleX) {
			for (int sampleY = 0; sampleY < _yPixelSamples; ++sampleY) {
				float deltaX = frand(), deltaY = frand();
				_samples2D[dimension][iSample2D++] = glm::vec2((sampleX + deltaX) * inv2DSamplesX, (sampleY + deltaY) * inv2DSamplesY);
			}
		}
		std::shuffle(_samples2D[dimension].begin(), _samples2D[dimension].end(), rngShuffle);
	}

	// Create 1D array sample values
	for (int i = 0; i < _sampleArray1D.size(); ++i) {
		size_t arraySize = _samples1DArraySizes[i];
		float invSampleArraySize = 1.f / arraySize;
		for (int j = 0; j < _samplesPerPixel; j++) {
			for (int sample = 0; sample < arraySize; ++sample) {
				float delta = frand();
				_sampleArray1D[i][j * arraySize + sample] = (sample + delta) * invSampleArraySize;
			}
			std::shuffle(_sampleArray1D[i].begin() + (arraySize * j), _sampleArray1D[i].begin() + (arraySize * (static_cast<size_t>(j) + 1)), rngShuffle);
		}
	}

	// Create 2D array sample values
	for (int i = 0; i < _sampleArray2D.size(); ++i) {
		size_t arraySize = _samples2DArraySizes[i];
		for (int j = 0; j < _samplesPerPixel; j++) {
			_latinHypercubeSampling2D(&_sampleArray2D[i][j * arraySize], static_cast<int>(arraySize));
		}
	}

	PixelSampler::startPixel(pixel);
}

void StratifiedSampler::_latinHypercubeSampling2D(glm::vec2* samplesArray, int nbSamples) {
	float inv2DSamples = 1.f / nbSamples;
	for (int i = 0; i < nbSamples; ++i) {
		float deltaX = frand(), deltaY = frand();
		samplesArray[i] = glm::vec2(inv2DSamples * (i + deltaX), inv2DSamples * (i + deltaY));
	}
	float* floatSamplesArray = (float*)samplesArray;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < nbSamples; ++j) {
			int other = j + int(frand() * (nbSamples - j));
			std::swap(floatSamplesArray[j * 2 + i], floatSamplesArray[other * 2 + i]);
		}
	}
}

Sampler* StratifiedSampler::clone(int seed) const {
	StratifiedSampler *clone = new StratifiedSampler(*this);
	clone->array1DOffset = 0;
	clone->array2DOffset = 0;
	clone->_current1DDimension = 0;
	clone->_current2DDimension = 0;
	clone->_currentPixel = glm::ivec2(0);
	clone->_currentPixelSampleIndex = 0;
	return clone;
}


