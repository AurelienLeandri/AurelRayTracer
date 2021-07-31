#pragma once
#include "SamplerIntegrator.h"

class WhittedIntegrator :
    public SamplerIntegrator
{
public:
    WhittedIntegrator(int maxDepth, std::shared_ptr<Camera> camera, std::shared_ptr<Sampler> sampler);

public:
    // Inherited via SamplerIntegrator
    virtual glm::vec3 Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, int depth = 0) const override;

private:
    int _maxDepth = 0;

};

