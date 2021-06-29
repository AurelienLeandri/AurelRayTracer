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
    struct Parameters {
        size_t width = Application::WIDTH;
        size_t height = Application::HEIGHT;
        size_t nbChannels = Application::NB_CHANNELS;
        size_t nbSamples = 2048;
        int nbThreads = 32;
    };

public:
    RayTracer(const Parameters& parameters = {});
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

    std::shared_ptr<Camera> _camera;

    const SceneData* _scene = nullptr;

    const Parameters _parameters;

    unsigned int _currentSample = 1;
    const size_t _portionSize;
};

