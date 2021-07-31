#include "SamplerIntegrator.h"

#include <ctime>
#include <iostream>

#include "Camera.h"
#include "Film.h"
#include "Sampler.h"
#include "Ray.h"
#include "BSDF.h"
#include "HitRecord.h"

SamplerIntegrator::SamplerIntegrator(std::shared_ptr<Sampler> sampler, std::shared_ptr<Camera> camera)
	: _sampler(sampler), _camera(camera)
{
}

void SamplerIntegrator::_renderTile(const glm::ivec2& tile) {
}

void SamplerIntegrator::render(const Scene& sceneData)
{
	preprocess(sceneData, *_sampler);

    std::clock_t start(std::clock());

    /*
    float buffer_factor = ((_currentSample - 1.f) / _currentSample);
    float color_factor = (1.f / _currentSample);
    */

    glm::ivec2 filmDimensions = _camera->getFilm()->getDimensions();
    const int tileSize = 16;
    glm::ivec2 nTiles2D((filmDimensions.x + tileSize - 1) / tileSize, (filmDimensions.y + tileSize - 1) / tileSize);
    int nTiles = nTiles2D.x * nTiles2D.y;

    //std::cout << "\tSample " << _currentSample;

#pragma omp parallel for num_threads(nTiles)
    for (int iTile = 0; iTile < nTiles; ++iTile) {
        glm::ivec2 tile(iTile % nTiles2D.x, iTile / nTiles2D.y);
        int samplerSeed = tile.x * tile.y * nTiles2D.x;
        std::unique_ptr<Sampler> tileSampler(_sampler->clone(samplerSeed));
        int x0 = tile.x * tileSize;
        int y0 = tile.y * tileSize;
        int x1 = glm::min(x0 + tileSize, filmDimensions.x);
        int y1 = glm::min(y0 + tileSize, filmDimensions.y);
        for (int i = y0; i < y1; ++i) {
            for (int j = x0; j < x1; ++j) {
                glm::ivec2 pixel(j, i);
                do {
                    CameraSample cameraSample = _sampler->generateCameraSample(pixel);
                    RayDifferential ray;
                    float rayWeight = _camera->generateRayDifferential(cameraSample, ray);
                    ray.scaleDifferentials(1 / std::sqrt(tileSampler->getSamplesPerPixel()));
                    glm::vec3 radiance(0);
                    if (rayWeight > 0) {
                        radiance = Li(ray, sceneData, *tileSampler);
                        if (radiance != radiance || radiance.x < 0 || radiance.y < 0 || radiance.z < 0) {
                            radiance = glm::vec3(1, 0, 0);  // red, just to distinguish between noise and errors.
                        }
                    }
                    _camera->getFilm()->addSample(pixel, radiance, rayWeight);
                } while (tileSampler->startNextSample());
            }
        }
    }

    //std::cout << " took " << (double(std::clock()) - start) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    /*
    ++_currentSample;
    return _currentSample > _NB_SAMPLES;
    */
}

void SamplerIntegrator::preprocess(const Scene& sceneData, Sampler& sampler)
{
}

glm::vec3 SamplerIntegrator::specularReflect(const RayDifferential& ray, const HitRecord& record, const Scene& scene, Sampler& sampler, int depth) const
{
    glm::vec3 wi(0, 0, 1);
    float pdf = 0;
    BxDF::Type type(BxDF::Type::BXDF_REFLECTION | BxDF::Type::BXDF_SPECULAR);

    glm::vec3 f = record.bsdf.sample_f(wi, -record.ray.direction, record, pdf, type);
    float dot_wi_normal = glm::abs(glm::dot(wi, record.normal));

    if (pdf > 0 && f != glm::vec3(0) && dot_wi_normal != 0) {
        RayDifferential rd;
        // TODO: compute ray differentials after reading the chapter
        return f * Li(rd, scene, sampler, depth + 1) * dot_wi_normal / pdf;
    }

    return glm::vec3(0);
}

glm::vec3 SamplerIntegrator::specularTransmit(const RayDifferential& ray, const HitRecord& record, const Scene& scene, Sampler& sampler, int depth) const
{
    glm::vec3 wi(0, 0, 1);
    float pdf = 0;
    BxDF::Type type(BxDF::Type::BXDF_TRANSMISSION | BxDF::Type::BXDF_SPECULAR);

    glm::vec3 f = record.bsdf.sample_f(wi, -record.ray.direction, record, pdf, type);
    float dot_wi_normal = glm::abs(glm::dot(wi, record.normal));

    if (pdf > 0 && f != glm::vec3(0) && dot_wi_normal != 0) {
        RayDifferential rd;
        // TODO: compute ray differentials after reading the chapter
        return f * Li(rd, scene, sampler, depth + 1) * dot_wi_normal / pdf;
    }

    return glm::vec3(0);
}
