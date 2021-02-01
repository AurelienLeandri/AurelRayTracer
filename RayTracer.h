#pragma once

#include <glm.hpp>
#include <omp.h>
#include <memory>

#include "Application.h"

class Hitable;
class Camera;
class Ray;

class RayTracer {
public:
    RayTracer() = default;
    ~RayTracer();

public:
    int init();
    bool iterate();
    const float* getImageBuffer() const;
    void setWorld(std::shared_ptr<Hitable> world);
    void setCamera(std::shared_ptr<Camera> camera);

private:
    glm::vec3 _getColor(const Ray& r, size_t depth) const;

private:
    float* _imageBuffer = nullptr;

    const unsigned int _WIDTH = Application::WIDTH;
    const unsigned int _HEIGHT = Application::HEIGHT;
    const unsigned int _NB_CHANNELS = Application::NB_CHANNELS;

    const unsigned int _NB_SAMPLES = 16;
    const float _INV_NB_SAMPLES = 1.f / _NB_SAMPLES;
    const unsigned int _NB_THREADS = omp_get_max_threads();
    const size_t _PORTION_SIZE = (_WIDTH * _HEIGHT) / _NB_THREADS;

    std::shared_ptr<Camera> _camera;
    std::shared_ptr<Hitable> _world;

    unsigned int _currentSample = 1;
};

