#pragma once

#include <glm/glm.hpp>

struct CameraSample {

};

class Sampler
{
public:
	Sampler *clone(int seed = 0) const;
	void startPixel(const glm::ivec2& pixel);
	CameraSample generateCameraSample(const glm::ivec2& pixel);
	int getSamplesPerPixel() const;
	bool startNextSample();

private:
	int _samplesPerPixel = 1;
};

