#include "RayTracer.h"

#include <glm.hpp>
#include <iostream>
#include <ctime>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Ray.h"
#include "Camera.h"
#include "Utils.h"
#include "HitRecord.h"
#include "HitableList.h"

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
            glm::vec3 sample_color = _getColor(r, 1);
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

glm::vec3 RayTracer::_getColor(const Ray& camera_ray, size_t depth) const {
    glm::vec3 color(0, 0, 0);
    glm::vec3 weights(1, 1, 1);
    Ray w_o = camera_ray;
    while (true) {
        HitRecord hit_record;
        if (_world->hit(w_o, hit_record)) {
            /*
            if (hit_record.material) {
                if (hit_record.material->albedo) {
                    return hit_record.material->albedo->color(hit_record.u, hit_record.v, hit_record.position);
                }
                else {
                    return hit_record.material->albedoValue;
                }
            }
            else {
                return hit_record.normal;
            }
            */
            if (!hit_record.material) {  // Default material
                static Material default_material;
                hit_record.material = &default_material;
                default_material.getBSDF(hit_record);
                default_material.emit(hit_record);
            }

            color += hit_record.emission * weights;
            if (depth-- == 0) break;

            const BSDF &bsdf = *hit_record.bsdf;
            glm::vec3 w_i(0, 0, 0);
            glm::vec3 sample_proba = bsdf.sample_f(w_i, -w_o.direction, hit_record);  // Get a sample vector, gets the proba to pick it
            glm::vec3 f = bsdf.f(w_i, -w_o.direction, hit_record);  // Scattering weight from BSDF
            w_i = glm::normalize(w_i);
            float cos_theta = std::fabs(glm::dot(w_i, hit_record.normal));  // Light attenuation wrt. angle
            weights *= (cos_theta * f) / sample_proba;
            w_o = Ray(hit_record.position, w_i);
        }
        else {
            break;
        }
    }
    return color;  // background black
}

const float* RayTracer::getImageBuffer() const
{
    return _imageBuffer;
}

void RayTracer::setWorld(const std::shared_ptr<Hitable> world, const std::vector<std::shared_ptr<Hitable>>& lights) {
    _world = world;
    _lights = lights;
}

void RayTracer::setCamera(std::shared_ptr<Camera> camera) {
    this->_camera = camera;
}
