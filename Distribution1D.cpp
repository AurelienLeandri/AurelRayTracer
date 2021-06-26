#include "Distribution1D.h"

#include <glm/glm.hpp>

Distribution1D::Distribution1D(const float* values, size_t n)
	: _values(values), _n(n), _cdf(_n + 1)
{
	float value_span = 1.f / _n;
	_cdf[0] = 0;
	for (size_t i = 0; i < _n; ++i) {
		_cdf[i + 1] = _cdf[i] + _values[i] * value_span;  // _values[i] * value_span gives the integral over a piece of the distribution.
	}

	// "values" is not in pdf form, so we need to divide each term of the cdf by the integral of values.
	// It is the same as computing a pdf vector for values (same as "values" but each element is divided by the integral)
	// and use it for computing the cdf, but we take advantage of the integral being a constant.
	integralValues = _cdf[_n];
	if (integralValues != 0) {
		float inv_integral_values = 1 / integralValues;
		for (size_t i = 0; i < _n; ++i) {
			_cdf[i] *= inv_integral_values;
		}
	}
}

float Distribution1D::sample(float e, int& index) const
{
	size_t i = _n / 2;
	size_t left = 0;
	size_t right = _n;
	while (i < _n - 1) {
		if (e < _cdf[i]) {
			right = i;
		}
		else if (e > _cdf[i + 1]) {
			left = i + 1;
		}
		else {
			break;
		}
		i = (left + right) / 2;
	}
	/*
	float du = glm::max(0.f, e - _cdf[i]);
	if (_cdf[i + 1] - _cdf[i] > 0)
		du /= _cdf[i + 1] - _cdf[i];
	*/
	index = i;
	return _values[i] / (integralValues);
}
