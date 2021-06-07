#include "SimpleRandomSampler.h"

#include "Utils.h"

#include <algorithm>
#include <random>

SimpleRandomSampler::SimpleRandomSampler(size_t xPixelSamples, size_t yPixelSamples, int nSampledDimensions)
	: PixelSampler(xPixelSamples* yPixelSamples, nSampledDimensions), _xPixelSamples(xPixelSamples), _yPixelSamples(yPixelSamples)
{
}

void SimpleRandomSampler::startPixel(const glm::ivec2& pixel)
{
	// Create 1D sample values
	for (int dimension = 0; dimension < _samples1D.size(); ++dimension) {
		for (int sample = 0; sample < _samplesPerPixel; ++sample) {
			_samples1D[dimension][sample] = frand();
		}
	}

	// Create 2D sample values
	for (int dimension = 0; dimension < _samples2D.size(); ++dimension) {
		for (int iSample2D = 0; iSample2D < _samplesPerPixel; ++iSample2D) {
			_samples2D[dimension][iSample2D] = glm::vec2(frand(), frand());
		}
	}

	// Create 1D array sample values
	for (int arrayIndex = 0; arrayIndex < _sampleArray1D.size(); ++arrayIndex) {
		int arraySize = _samples1DArraySizes[arrayIndex];
		for (int j = 0; j < _samplesPerPixel; j++) {
			for (int sample = 0; sample < arraySize; ++sample) {
				_sampleArray1D[arrayIndex][j * arraySize + sample] = frand();
			}
		}
	}

	// Create 2D array sample values
	for (int arrayIndex = 0; arrayIndex < _sampleArray2D.size(); ++arrayIndex) {
		int arraySize = _samples2DArraySizes[arrayIndex];
		for (int s = 0; s < _samplesPerPixel; ++s) {
			for (int j = 0; j < arraySize; ++j) {
				_sampleArray2D[arrayIndex][s * arraySize + j] = glm::vec2(frand(), frand());
			}
		}
	}

	PixelSampler::startPixel(pixel);
}

Sampler* SimpleRandomSampler::clone(int seed) const {
	SimpleRandomSampler* clone = new SimpleRandomSampler(*this);
	clone->array1DOffset = 0;
	clone->array2DOffset = 0;
	clone->_current1DDimension = 0;
	clone->_current2DDimension = 0;
	clone->_currentPixel = glm::ivec2(0);
	clone->_currentPixelSampleIndex = 0;
	return clone;
}
