#include "MicrofacetDistribution.h"

#include <glm.hpp>

MicrofacetDistribution::MicrofacetDistribution()
{
}

float MicrofacetDistribution::roughnessToAlpha(float roughness)
{
    // Copypasta from pbrt, because the roughness value is much more convenient to use and represent to oneself
    roughness = glm::min(float(1), glm::max(roughness, 1e-3f));  // Clamp roughness values within ]0, 1]
    float x = glm::log(roughness);
    return 1.62142f + 0.819955f * x + 0.1734f * x * x +
        0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
}
