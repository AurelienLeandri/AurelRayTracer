#pragma once
#include "MicrofacetDistribution.h"
class BeckmannSpizzichino :
    public MicrofacetDistribution
{
public:
    BeckmannSpizzichino(float alpha);
    BeckmannSpizzichino(float alpha_x, float alpha_y);

public:
    virtual float D(const glm::vec3& w_h) override;

private:
    float _alphaX = 0;
    float _alphaY = 0;
};

