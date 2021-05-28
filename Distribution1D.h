#pragma once

#include <vector>

class Distribution1D
{
public:
	Distribution1D(const float* values, size_t n);
	float sample(float e, int& index) const;

public:
	float integralValues= 0;

private:
	const size_t _n;
	const float* _values;
	std::vector<float> _cdf;
};

