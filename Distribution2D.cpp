#include "Distribution2D.h"

#include "Utils.h"

Distribution2D::Distribution2D(const float* values, size_t nu, size_t nv)
    : _values(values), _nu(nu), _nv(nv)
{
    for (int u = 0; u < _nu; ++u) {
        _conditionalDistributionU.push_back(std::make_unique<Distribution1D>(&values[u * _nv], _nv));
    }
    for (int u = 0; u < _nu; ++u) {
        _marginalDensityVValues.push_back(_conditionalDistributionU[u]->integralValues);
    }
    _marginalDistributionV = std::make_unique<Distribution1D>(&_marginalDensityVValues[0], _nu);
}

float Distribution2D::sample(float e1, float e2, size_t& u, size_t& v) const
{
    float marginal_density_v = _marginalDistributionV->sample(e1, u);
    float conditional_density_u = _conditionalDistributionU[u]->sample(e2, v);
    return conditional_density_u * marginal_density_v;
}
