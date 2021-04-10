#pragma once
#include "MicrofacetDistribution.h"
class BeckmannSpizzichino :
    public MicrofacetDistribution
{
public:
    BeckmannSpizzichino(float alpha);
    BeckmannSpizzichino(float alpha_x, float alpha_y);

public:
    virtual float D(const glm::vec3& w_h) const override;
    virtual void sample_wh(glm::vec3& w_h) const;

private:
    float _alphaX = 0;
    float _alphaY = 0;

    // Inherited via MicrofacetDistribution
    virtual float lambda(const glm::vec3& w) const override;
};

