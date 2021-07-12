#include "RayTracer.h"

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

RayTracer::RayTracer(const Parameters& parameters)
    : _parameters(parameters), _portionSize((_parameters.width * _parameters.height) / _parameters.nbThreads)
{
}

RayTracer::~RayTracer()
{
    if (_imageBuffer) {
        delete[] _imageBuffer;
        _imageBuffer = nullptr;
    }
}

int RayTracer::init()
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

bool RayTracer::iterate() {
    std::cout << "\tSample " << _currentSample;

    std::clock_t start(std::clock());

    float buffer_factor = ((_currentSample - 1.f) / _currentSample);
    float color_factor = (1.f / _currentSample);

#pragma omp parallel for num_threads(NB_THREADS)
    for (unsigned int portion = 0; portion < _parameters.nbThreads; portion++) {
        for (size_t i = portion * _portionSize; i < (portion + 1) * _portionSize; i++) {
            glm::vec3 pixel_screen_position = glm::vec3((static_cast<float>(i % _parameters.width) / _parameters.width), (1.f - (static_cast<float>(i / _parameters.width) / _parameters.height)), 0.f);  // y pointing upward
            glm::vec3 sample_screen_position = pixel_screen_position + glm::vec3(frand() / _parameters.width, frand() / _parameters.height, 0.f);
            Ray r = _camera->getRay(sample_screen_position.x, sample_screen_position.y);
            glm::vec3 sample_color = glm::max(glm::vec3(0), _getColor(r, 1));
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


glm::vec3 RayTracer::_getColor(const Ray& camera_ray, size_t max_depth) const {
    size_t depth = 0;
    glm::vec3 color(0, 0, 0);
    glm::vec3 path_accumulated_weight(1, 1, 1);
    Ray w_o = camera_ray;
    BxDF::Type last_bxdf_used = BxDF::Type::BXDF_NONE;
    while (!max_depth || depth++ < max_depth) {
        HitRecord hit_record;
        if (_scene->castRay(w_o, hit_record)) {

            // We add the emission at intersection in two exceptional cases:
            // 1 - Rays starting from the camera that hit a light source immediately
            // 2 - If the BxDF at the last surface intersection was specular, direct lighting contribution was not taken into account (f is always 0)
            if (depth == 1 || (last_bxdf_used.flags & BxDF::Type::BXDF_SPECULAR)) {
                color += hit_record.emission;
            }

            glm::vec3 w_o_calculations = glm::normalize(-w_o.direction);

            // Add contribution of direct lighting to the path:
            // Light sampling using light distribution
            const std::vector<std::shared_ptr<const Light>>& sceneLights = _scene->getLights();
            int nbLights = sceneLights.size();
            std::shared_ptr<const Light> light = sceneLights[int(frand() * nbLights)];
            float pdfDirectLighting = 0;
            glm::vec3 light_sample_dl(0, 0, 1);
            glm::vec3 radiance = light->sampleLi(light_sample_dl, hit_record, pdfDirectLighting);
            Ray direct_lighting_ray(hit_record.position, light_sample_dl);
            light_sample_dl = glm::normalize(light_sample_dl);
            HitRecord occlusion_hit_record;
            glm::vec3 light_color(0);
            if (light->getType() == LightType::AREA) {
                // If the light can be sampled from our position, we check if we hit the light:
                // To verify this, "occlusion_hit_record.tRay" should be very close to one since "light_sample" stretches from the current position to the light.
                if (pdfDirectLighting > 0 && _scene->castRay(direct_lighting_ray, occlusion_hit_record) && occlusion_hit_record.tRay >= 0.9999f) {
                    float cos_light_surface = glm::dot(light_sample_dl, hit_record.normal);
                    if (cos_light_surface > 0) {
                        glm::vec3 light_f = hit_record.bsdf.f(light_sample_dl, w_o_calculations, hit_record);
                        glm::vec3 light_scattering = light_f * cos_light_surface;  // The direct lighing is affected by the surface properties and by the cos factor
                        light_color = radiance * ((path_accumulated_weight * light_scattering) / pdfDirectLighting) * float(nbLights);
                    }
                }
            }
            else if (light->getType() == LightType::INFINITE_AREA) {
                if (pdfDirectLighting > 0 && !_scene->castRay(direct_lighting_ray, occlusion_hit_record)) {
                    float cos_light_surface = glm::dot(light_sample_dl, hit_record.normal);
                    if (cos_light_surface > 0) {
                        glm::vec3 light_f = hit_record.bsdf.f(light_sample_dl, w_o_calculations, hit_record);
                        glm::vec3 light_scattering = light_f * cos_light_surface;  // The direct lighing is affected by the surface properties and by the cos factor
                        light_color = radiance * ((path_accumulated_weight * light_scattering) / pdfDirectLighting) * float(nbLights);
                    }
                }
            }

            // Light sampling using BSDF
            glm::vec3 bsdf_color(0);
            glm::vec3 light_sample_bsdf(0, 0, 1);
            float pdfBSDF = 0;
            glm::vec3 bsdf_scattering = hit_record.bsdf.sample_f(light_sample_bsdf, w_o_calculations, hit_record, pdfBSDF, last_bxdf_used);  // Get a sample vector, gets the proba to pick it
            light_sample_bsdf = glm::normalize(light_sample_bsdf);
            Ray bsdf_light_sampling_ray(hit_record.position, light_sample_bsdf);
            HitRecord bsdf_sample_hit_record;
            if (pdfBSDF > 0 && _scene->castRay(bsdf_light_sampling_ray, bsdf_sample_hit_record)) {
                bsdf_color = bsdf_sample_hit_record.emission;
            }
            else {
                bsdf_color = _scene->getEnvironmentLight()->radianceInDirection(light_sample_bsdf);
            }
            bsdf_color *= path_accumulated_weight * (glm::abs(glm::dot(hit_record.normal, light_sample_bsdf)) * bsdf_scattering) / pdfBSDF;

            color += Power2Heuristic(1, pdfDirectLighting, 1, pdfBSDF) * light_color + Power2Heuristic(1, pdfBSDF, 1, pdfDirectLighting) * bsdf_color;

            // Computing the next step of the path and updating the accumulated weight
            glm::vec3 w_i(0, 0, 0);
            float sample_proba = 0;
            glm::vec3 f = hit_record.bsdf.sample_f(w_i, w_o_calculations, hit_record, sample_proba, last_bxdf_used);  // Get a sample vector, gets the proba to pick it

            if (sample_proba == 0 || f == glm::vec3(0))
                break;

            w_o = Ray(hit_record.position, w_i);
            float light_attenuation_wrt_angle = std::fabs(glm::dot(glm::normalize(w_i), hit_record.normal));
            path_accumulated_weight *= (light_attenuation_wrt_angle * f) / sample_proba;

            // Russian roulette
            if (depth >= 21) {  // 4
                float russian_roulette_weight = glm::max(0.05f, 1 - (0.3f * path_accumulated_weight.r + 0.59f * path_accumulated_weight.g + 0.11f * path_accumulated_weight.b));
                if (frand() < russian_roulette_weight) {
                    break;
                }
                path_accumulated_weight /= 1 - russian_roulette_weight;
            }
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

const float* RayTracer::getImageBuffer() const
{
    return _imageBuffer;
}

void RayTracer::setCamera(std::shared_ptr<Camera> camera) {
    _camera = camera;
}

void RayTracer::setScene(const SceneData& scene) {
    _scene = &scene;
}

namespace { // embree

    void errorFunction(void* userPtr, enum RTCError error, const char* str)
    {
        printf("error %d: %s\n", error, str);
    }

}

bool RayTracer::start() {
    if (!_scene || !_camera) {
        return false;
    }
    
    return true;
}

