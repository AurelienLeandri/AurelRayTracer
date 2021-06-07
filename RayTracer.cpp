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

RayTracer::~RayTracer()
{
    if (_imageBuffer) {
        delete[] _imageBuffer;
        _imageBuffer = nullptr;
    }
}

int RayTracer::init()
{
    _imageBuffer = new float[_WIDTH * _HEIGHT * _NB_CHANNELS];
    for (int i = 0; i < _WIDTH * _HEIGHT * _NB_CHANNELS; ++i)
        _imageBuffer[i] = 1.f;

    return 0;
}

bool RayTracer::iterate() {
    std::cout << "\tSample " << _currentSample;

    std::clock_t start(std::clock());

    float buffer_factor = ((_currentSample - 1.f) / _currentSample);
    float color_factor = (1.f / _currentSample);

#pragma omp parallel for num_threads(NB_THREADS)
    for (unsigned int portion = 0; portion < _NB_THREADS; portion++) {
        for (size_t i = portion * _PORTION_SIZE; i < (portion + 1) * _PORTION_SIZE; i++) {
            glm::vec3 pixel_screen_position = glm::vec3((static_cast<float>(i % _WIDTH) / _WIDTH), (1.f - (static_cast<float>(i / _WIDTH) / _HEIGHT)), 0.f);  // y pointing upward
            glm::vec3 sample_screen_position = pixel_screen_position + glm::vec3(frand() / _WIDTH, frand() / _HEIGHT, 0.f);
            Ray r = _camera->getRay(sample_screen_position.x, sample_screen_position.y);
            glm::vec3 sample_color = glm::max(glm::vec3(0), _getColor(r, 10));
            glm::vec3 previous_color(_imageBuffer[(i * 3)], _imageBuffer[(i * 3) + 1], _imageBuffer[(i * 3) + 2]);
            glm::vec3 color = buffer_factor * previous_color + color_factor * sample_color;
            _imageBuffer[(i * 3)] = color.r;
            _imageBuffer[(i * 3) + 1] = color.g;
            _imageBuffer[(i * 3) + 2] = color.b;
        }
    }

    std::cout << " took " << (double(std::clock()) - start) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    ++_currentSample;
    return _currentSample > _NB_SAMPLES;
}


glm::vec3 RayTracer::_getColor(const Ray& camera_ray, size_t max_depth) const {
    size_t depth = 0;
    glm::vec3 color(0, 0, 0);
    glm::vec3 path_accumulated_weight(1, 1, 1);
    Ray w_o = camera_ray;
    //float russian_roulette_weight = 1.f;
        while (!max_depth || depth++ < max_depth) {
        HitRecord hit_record;
        if (_scene->castRay(w_o, hit_record)) {

            if (depth == 1) {  // Hitting lights on the first ray
                color += hit_record.emission;
            }

            glm::vec3 w_o_calculations = glm::normalize(-w_o.direction);

            // Direct lighting
            for (std::shared_ptr<const Light> light : _scene->getLights()) {
                float light_sample_proba = 0;
                glm::vec3 light_sample(0, 0, 1);
                glm::vec3 radiance = light->sampleLi(light_sample, hit_record, light_sample_proba);
                Ray direct_lighting_ray(hit_record.position, light_sample);
                light_sample = glm::normalize(light_sample);
                HitRecord occlusion_hit_record;
                if (light->getType() == LightType::AREA) {
                    // If the light can be sampled from our position, we check if we hit the light:
                    // To verify this, "occlusion_hit_record.tRay" should be very close to one since "light_sample" stretches from the current position to the light.
                    if (light_sample_proba > 0.000001f && _scene->castRay(direct_lighting_ray, occlusion_hit_record) && occlusion_hit_record.tRay >= 0.9999f) {
                        float cos_light_surface = glm::dot(light_sample, hit_record.normal);
                        if (cos_light_surface > 0) {
                            glm::vec3 light_f = hit_record.bsdf.f(light_sample, w_o_calculations, hit_record);
                            glm::vec3 light_scattering = light_f * cos_light_surface;  // The direct lighing is affected by the surface properties and by the cos factor
                            glm::vec3 light_color = radiance * path_accumulated_weight * light_scattering / light_sample_proba;
                            color += light_color;
                        }
                    }
                }/*
                else if (light->getType() == LightType::INFINITE_AREA) {
                    if (light_sample_proba > 0.000001f && !_scene->castRay(direct_lighting_ray, occlusion_hit_record)) {
                        float cos_light_surface = glm::dot(light_sample, hit_record.normal);
                        if (cos_light_surface > 0) {
                            glm::vec3 light_f = hit_record.bsdf.f(light_sample, w_o_calculations, hit_record);
                            glm::vec3 light_scattering = light_f * cos_light_surface;  // The direct lighing is affected by the surface properties and by the cos factor
                            glm::vec3 light_color = radiance * path_accumulated_weight * light_scattering / light_sample_proba;
                            color += light_color;
                        }
                    }
                }
                */
            }

            // Computing the next step of the path and updating the accumulated weight
            glm::vec3 w_i(0, 0, 0);
            float sample_proba = 0;
            glm::vec3 f = hit_record.bsdf.sample_f(w_i, w_o_calculations, hit_record, sample_proba);  // Get a sample vector, gets the proba to pick it

            if (sample_proba < 0.000001f || (f == glm::vec3(0)))
                break;

            w_o = Ray(hit_record.position, w_i);

            float light_attenuation_wrt_angle = std::fabs(glm::dot(glm::normalize(w_i), hit_record.normal));
            if (glm::abs(sample_proba - light_attenuation_wrt_angle) < 1e-6f)
                continue;


            /*
            if (depth >= 5) {
                russian_roulette_weight = glm::max(0.f, 1.f - glm::min(0.0625f * depth, 1.f));
                if (frand() > russian_roulette_weight) {
                    break;
                }
            }
            */

            glm::vec3 scattering = light_attenuation_wrt_angle * f;

            glm::vec3 previous_weight = path_accumulated_weight;
            path_accumulated_weight *= scattering / sample_proba;
            //path_accumulated_weight /= russian_roulette_weight;
        }
        else {
            /*
            if (depth > 1)
                break;
            */
            static std::shared_ptr<ImageTexture> environment_emission_texture = std::make_shared<ImageTexture>("lakeside_2k.hdr");
            float u = 0, v = 0;
            get_sphere_uv(glm::normalize(w_o.direction), u, v);
            HitRecord r;
            r.u = u;
            r.v = v;
            color += path_accumulated_weight * environment_emission_texture->getColor(r);
            break;  // The path is over. The ray didn't hit anything
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

