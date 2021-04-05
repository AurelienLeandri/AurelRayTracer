#pragma once
#include "MicrofacetDistribution.h"
class TrowbridgeReitz :
    public MicrofacetDistribution
{
public:
    TrowbridgeReitz(float alpha);
    TrowbridgeReitz(float alpha_x, float alpha_y);

public:
    virtual float D(const glm::vec3& w_h) override;

private:
    float _alphaX = 0;
    float _alphaY = 0;
};

