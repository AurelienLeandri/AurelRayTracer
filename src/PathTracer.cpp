#include "PathTracer.h"

#include <glm/glm.hpp>
#include <iostream>
#include <ctime>
#include <unordered_map>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Ray.h"
#include "Camera.h"
#include "Utils.h"
#include "HitRecord.h"
#include "Embree.h"
#include "Texture.h"
#include "Scene.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"
#include "Triangle.h"


static float max_green = 0;

PathTracer::PathTracer(const Parameters& parameters)
    : _parameters(parameters), _portionSize((_parameters.width * _parameters.height) / _parameters.nbThreads)
{
}

PathTracer::~PathTracer()
{
    if (_imageBuffer) {
        delete[] _imageBuffer;
        _imageBuffer = nullptr;
    }
}

int PathTracer::init()
{
    if (_imageBuffer) {
        delete[] _imageBuffer;
        _imageBuffer = nullptr;
    }

    _imageBuffer = new float[_parameters.width * _parameters.height * _parameters.nbChannels];
    for (int i = 0; i < _parameters.width * _parameters.height * _parameters.nbChannels; ++i)
        _imageBuffer[i] = 1.f;

    return 0;
}

bool PathTracer::iterate() {
    std::cout << "\tSample " << _currentSample;

    std::clock_t start(std::clock());

    float buffer_factor = ((_currentSample - 1.f) / _currentSample);
    float color_factor = (1.f / _currentSample);
    int portionSize = static_cast<int>(_portionSize);

    //srand(time(NULL));

#pragma omp parallel for num_threads(NB_THREADS)
    for (int portion = 0; portion < _parameters.nbThreads; portion++) {
        for (int i = portion * portionSize; i < (portion + 1) * portionSize; i++) {
            glm::vec3 pixel_screen_position = glm::vec3((static_cast<float>(i % _parameters.width) / _parameters.width), (1.f - (static_cast<float>(i / _parameters.width) / _parameters.height)), 0.f);  // y pointing upward
            glm::vec3 sample_screen_position = pixel_screen_position + glm::vec3(frand() / _parameters.width, frand() / _parameters.height, 0.f);
            Ray r = _camera->getRay(sample_screen_position.x, sample_screen_position.y);
            glm::vec3 sample_color = glm::max(glm::vec3(0), _getColor(r, _parameters.maxDepth));
            glm::vec3 previous_color(_imageBuffer[(i * 3)], _imageBuffer[(i * 3) + 1], _imageBuffer[(i * 3) + 2]);
            glm::vec3 color = buffer_factor * previous_color + color_factor * sample_color;
            _imageBuffer[(i * 3)] = color.r;
            _imageBuffer[(i * 3) + 1] = color.g;
            _imageBuffer[(i * 3) + 2] = color.b;
        }
    }

    std::cout << " took " << (double(std::clock()) - start) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    ++_currentSample;
    return _currentSample > _parameters.nbSamples;
}

// NOTE: taken from pbr
float Power2Heuristic(int nf, float fPdf, int ng, float gPdf) {
    float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}

glm::vec3 PathTracer::_importanceSamplingRadiance(const glm::vec3 &wo, const glm::vec3 &pathWeight, const HitRecord &surfaceRecord, const SamplingStrategy& strategy) const {
    glm::vec3 light_color(0);

    const std::vector<std::shared_ptr<const Light>>& sceneLights = _scene->getLights();
    int nbLights = static_cast<int>(sceneLights.size());
    std::shared_ptr<const Light> light = sceneLights[static_cast<int>(static_cast<double>(frand()) * nbLights)];

    if ((strategy & (SamplingStrategy::LightsAndBSDF | SamplingStrategy::LightsOnly)) && _scene->getLights().size()) {
        // Light sampling using light distribution
        if (!(surfaceRecord.areaLight.get() == light.get())) {  // Exclude the case where we're sampling a light we are curently on.
            glm::vec3 lightSample(0, 0, 1);
            float pdfLight = 0;
            glm::vec3 radiance = light->sampleLi(lightSample, surfaceRecord, pdfLight);
            if (radiance != glm::vec3(0) && pdfLight > 0) {
                Ray direct_lighting_ray(surfaceRecord.position, lightSample);
                lightSample = glm::normalize(lightSample);
                HitRecord occlusion_hit_record;
                glm::vec3 f(0);
                float pdfBSDF = 0;
                float lightAttenuationWrtAngle = glm::dot(lightSample, surfaceRecord.normal);
                if (lightAttenuationWrtAngle > 0) {
                    f = surfaceRecord.bsdf.f(lightSample, wo, surfaceRecord);
                    pdfBSDF = surfaceRecord.bsdf.pdf(lightSample, wo, surfaceRecord);

                    if (f != glm::vec3(0) && pdfBSDF > 0) {
                        // Occlusion test
                        bool isLightReachable =
                            // Area light that can be hit from our position
                            light->getType() == LightType::AREA
                            && _scene->castRay(direct_lighting_ray, occlusion_hit_record)
                            && occlusion_hit_record.tRay >= 0.9999f
                            ||
                            // Infinite area light and nothing is hit along the sampled ray
                            light->getType() == LightType::INFINITE_AREA
                            && !_scene->castRay(direct_lighting_ray, occlusion_hit_record);

                        if (isLightReachable) {
                            float power2HeuristicWeight = strategy & SamplingStrategy::LightsAndBSDF ? Power2Heuristic(1, pdfLight, 1, pdfBSDF) : 1;
                            light_color += f * radiance * power2HeuristicWeight / pdfLight;
                        }
                    }
                }
            }
        }
    }

    if (strategy & (SamplingStrategy::LightsAndBSDF | SamplingStrategy::BSDFOnly)) {
        // Light sampling using BSDF
        glm::vec3 bsdfSample(0, 0, 1);
        BxDF::Type bxdfType = BxDF::Type::BXDF_NONE;
        float pdfBSDF = 0;
        glm::vec3 f = surfaceRecord.bsdf.sample_f(bsdfSample, wo, surfaceRecord, pdfBSDF, bxdfType);
        bsdfSample = glm::normalize(bsdfSample);
        float lightAttenuationWrtAngle = glm::dot(surfaceRecord.normal, bsdfSample);
        if (lightAttenuationWrtAngle > 0 && f != glm::vec3(0) && pdfBSDF > 0) {
            Ray bsdfRaySample(surfaceRecord.position, bsdfSample);
            HitRecord bsdf_sample_hit_record;
            glm::vec3 radianceFromLight(0);
            float lightPdf = 0;
            if (_scene->castRay(bsdfRaySample, bsdf_sample_hit_record) && bsdf_sample_hit_record.areaLight.get() == light.get()) {
                radianceFromLight = bsdf_sample_hit_record.emission;
                lightPdf = light->pdf(bsdf_sample_hit_record.position - surfaceRecord.position, surfaceRecord);
            }
            else {
                const InfiniteAreaLight* environmentLight = _scene->getEnvironmentLight();
                if (environmentLight && environmentLight == light.get()) {
                    radianceFromLight = _scene->getEnvironmentLight()->radianceInDirection(bsdfSample);
                    lightPdf = environmentLight->pdf(bsdfSample, surfaceRecord);
                }
            }
            if (lightPdf > 0) {
                float power2HeuristicWeight = strategy & SamplingStrategy::LightsAndBSDF ? Power2Heuristic(1, pdfBSDF, 1, lightPdf) : 1;
                light_color += radianceFromLight * power2HeuristicWeight * lightAttenuationWrtAngle * f / pdfBSDF;
            }
        }
    }

    return static_cast<float>(nbLights) * light_color;
}

glm::vec3 PathTracer::_getColor(const Ray& camera_ray, size_t max_depth) const {
    size_t depth = 0;
    glm::vec3 color(0, 0, 0);
    glm::vec3 path_accumulated_weight(1, 1, 1);
    Ray w_o = camera_ray;
    BxDF::Type last_bxdf_used = BxDF::Type::BXDF_NONE;
    while (!max_depth || depth < max_depth) {
        depth++;
        HitRecord hit_record;
        if (_scene->castRay(w_o, hit_record)) {
            if (depth == 3) {
                int a = 0;
            }

            // We add the emission at intersection in two exceptional cases:
            // 1 - Rays starting from the camera that hit a light source immediately
            // 2 - If the BxDF at the last surface intersection was specular, direct lighting contribution was not taken into account (f is always 0)
            if (depth == 1 || (last_bxdf_used.flags & BxDF::Type::BXDF_SPECULAR)) {
                color += path_accumulated_weight * hit_record.emission;
            }

            glm::vec3 w_o_calculations = glm::normalize(-w_o.direction);

            color += path_accumulated_weight * _importanceSamplingRadiance(w_o_calculations, path_accumulated_weight, hit_record, _parameters.strategy);

            // Computing the next step of the path and updating the accumulated weight
            glm::vec3 w_i(0, 0, 0);
            float sample_proba = 0;
            glm::vec3 f = hit_record.bsdf.sample_f(w_i, w_o_calculations, hit_record, sample_proba, last_bxdf_used);  // Get a sample vector, gets the proba to pick it
            w_i = glm::normalize(w_i);
            float light_attenuation_wrt_angle = glm::abs(glm::dot(w_i, hit_record.normal));

            if (sample_proba == 0 || f == glm::vec3(0))
                break;

            w_o = Ray(hit_record.position, w_i);
            path_accumulated_weight = path_accumulated_weight * (light_attenuation_wrt_angle * f) / sample_proba;
        }
        else {
            // Same conditions as earlier to add the environment light contribution.
            if (depth == 1 || (last_bxdf_used.flags & BxDF::Type::BXDF_SPECULAR)) {
                const InfiniteAreaLight* environmentLight = _scene->getEnvironmentLight();
                if (environmentLight) {
                    color += path_accumulated_weight * environmentLight->radianceInDirection(w_o.direction);
                }
            }
            break;
        }

    }
    return color;
}

const float* PathTracer::getImageBuffer() const
{
    return _imageBuffer;
}

void PathTracer::setCamera(std::shared_ptr<Camera> camera) {
    _camera = camera;
}

void PathTracer::setScene(const SceneData& scene) {
    _scene = &scene;
}

namespace { // embree

    void errorFunction(void* userPtr, enum RTCError error, const char* str)
    {
        printf("error %d: %s\n", error, str);
    }

}

bool PathTracer::start() {
    if (!_scene || !_camera) {
        return false;
    }
    
    return true;
}

