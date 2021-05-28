#pragma once

#include <vector>
#include <memory>

#include"Distribution1D.h"

class Distribution2D
{
public:
	Distribution2D(const float* values, size_t nu, size_t nv);
	float sample(float e1, float e2, int& u, int& v) const;

private:
	const float* _values;
	size_t _nu = 0;
	size_t _nv = 0;
	std::vector<float> _marginalDensityVValues;;
	std::unique_ptr<Distribution1D> _marginalDistributionV;
	std::vector<std::unique_ptr<Distribution1D>> _conditionalDistributionU;
};

