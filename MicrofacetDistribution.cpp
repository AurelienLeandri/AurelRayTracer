#include "MicrofacetDistribution.h"

#include <glm.hpp>
#include <iostream>

MicrofacetDistribution::MicrofacetDistribution()
{
    /*
    std::cout << std::endl;
    std::cout << "Roughness to alpha" << std::endl;
    float step = 1.f / 100;
    float r = 0;
    while (r < 1) {
        std::cout << roughnessToAlpha(r) << std::endl;
        r += step;
    }
    */
}

float MicrofacetDistribution::G1(const glm::vec3& w) const
{
    return 1 / (1 + lambda(w));
}

float MicrofacetDistribution::G(const glm::vec3& wo, const glm::vec3& wi) const
{
    // Assuming than G2(wo) and G2(wi) are independent, we could return G(wo)*G(wi). But in practice this is wrong (ex wo = wi).
    // The formula below is a nice approximation (not explained for now)
    return 1 / (1 + lambda(wo) + lambda(wi));
}

float MicrofacetDistribution::roughnessToAlpha(float roughness)
{
    // Copypasta from pbrt, because the roughness value is much more convenient to use and represent to oneself
    roughness = glm::min(float(1), glm::max(roughness, 1e-3f));  // Clamp roughness values within ]0, 1]
    float x = glm::log(roughness);
    return 1.62142f + 0.819955f * x + 0.1734f * x * x +
        0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
}
