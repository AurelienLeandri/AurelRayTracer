#pragma once

#include <glm.hpp>
#include <omp.h>
#include <memory>
#include <vector>

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
    void setWorld(std::shared_ptr<Hitable> world, const std::vector<std::shared_ptr<Hitable>> &lights);
    void setCamera(std::shared_ptr<Camera> camera);

private:
    glm::vec3 _getColor(const Ray& r, size_t depth) const;

private:
    float* _imageBuffer = nullptr;

    const size_t _WIDTH = Application::WIDTH;
    const size_t _HEIGHT = Application::HEIGHT;
    const size_t _NB_CHANNELS = Application::NB_CHANNELS;

    const size_t _NB_SAMPLES = 1000;
    const float _INV_NB_SAMPLES = 1.f / _NB_SAMPLES;
    const size_t _NB_THREADS = omp_get_max_threads();
    const size_t _PORTION_SIZE = (_WIDTH * _HEIGHT) / _NB_THREADS;

    std::shared_ptr<Camera> _camera;
    std::shared_ptr<Hitable> _world;

    std::vector<std::shared_ptr<Hitable>> _lights;

    unsigned int _currentSample = 1;
};

