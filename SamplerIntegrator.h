#pragma once
#include "IntegratorOld.h"

#include <glm/glm.hpp>

#include <memory>

class Sampler;
class Camera;
class RayDifferential;
struct HitRecord;

class SamplerIntegrator :
    public IntegratorOld
{
public:
    SamplerIntegrator(std::shared_ptr<Sampler> sampler, std::shared_ptr<Camera> camera);

public:
    // Inherited via Integrator
    virtual void render(const Scene& sceneData) override;

    // Do some preprocessing on the scene. Called after the scene has been initialized.
    virtual void preprocess(const Scene& sceneData, Sampler& sampler);

    // Returns incoming radiance from a given camera ray differential
    virtual glm::vec3 Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, int depth = 0) const = 0;

    // Sends a ray towards the specular reflection direction
    glm::vec3 specularReflect(const RayDifferential& ray, const HitRecord& isect, const Scene& scene, Sampler& sampler, int depth) const;

    // Sends a ray towards the specular transmission direction
    glm::vec3 specularTransmit(const RayDifferential& ray, const HitRecord& isect, const Scene& scene, Sampler& sampler, int depth) const;

private:
    void _renderTile(const glm::ivec2 &tile);

private:
    std::shared_ptr<Sampler> _sampler;
    std::shared_ptr<Camera> _camera;
};

