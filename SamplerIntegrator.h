#pragma once
#include "Integrator.h"

#include <glm/glm.hpp>

#include <memory>

class Sampler;
class Camera;
class RayDifferential;

class SamplerIntegrator :
    public Integrator
{
public:
    SamplerIntegrator(std::shared_ptr<Sampler> sampler, std::shared_ptr<Camera> camera);

public:
    // Inherited via Integrator
    virtual void render(const SceneData& sceneData) override;

    // Do some preprocessing on the scene. Called after the scene has been initialized.
    virtual void preprocess(const SceneData& sceneData, Sampler& sampler);

    // Returns incoming radiance from a given camera ray differential
    virtual glm::vec3 Li(const RayDifferential& ray, const SceneData& scene, Sampler& sampler, int depth = 0) const = 0;

private:
    void _renderTile(const glm::ivec2 &tile);

private:
    std::shared_ptr<Sampler> _sampler;
    std::shared_ptr<Camera> _camera;
};

