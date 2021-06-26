#pragma once

#include <glm/glm.hpp>
#include <omp.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <embree3/rtcore.h>

#include "Application.h"
#include "Vertex.h"

class Camera;
class Ray;
struct HitRecord;
class Material;
class SceneData;
class Light;

class RayTracer {
public:
    RayTracer() = default;
    ~RayTracer();

public:
    int init();
    bool iterate();
    const float* getImageBuffer() const;
    void setCamera(std::shared_ptr<Camera> camera);
    void setScene(const SceneData& scene);
    bool start();


private:
    glm::vec3 _getColor(const Ray& r, size_t max_depth = 0) const;

private:
    float* _imageBuffer = nullptr;

    const size_t _WIDTH = Application::WIDTH;
    const size_t _HEIGHT = Application::HEIGHT;
    const size_t _NB_CHANNELS = Application::NB_CHANNELS;

    const size_t _NB_SAMPLES = 1024;
    const float _INV_NB_SAMPLES = 1.f / _NB_SAMPLES;
    const size_t _NB_THREADS = omp_get_max_threads();
    const size_t _PORTION_SIZE = (_WIDTH * _HEIGHT) / _NB_THREADS;

    std::shared_ptr<Camera> _camera;

    const SceneData* _scene = nullptr;

    unsigned int _currentSample = 1;
};

