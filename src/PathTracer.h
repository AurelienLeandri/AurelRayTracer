#pragma once

#include <glm/glm.hpp>
#include <omp.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <embree3/rtcore.h>

#include "Application.h"
#include "Vertex.h"
#include "BxDF.h"

class Camera;
class Ray;
struct HitRecord;
class Material;
class SceneData;
class Light;

class PathTracer {
public:
    enum ImportanceSamplingStrategy {
        LightsOnly = 1 << 0,
        BSDFOnly = 1 << 1,
        LightsAndBSDF = 1 << 2
    };

    enum class IntegratorStrategy {
        SimplePathTracer,
        PathTracerDirectLighting
    };

public:
    struct Parameters {
        size_t width = Application::WIDTH;
        size_t height = Application::HEIGHT;
        size_t nbChannels = Application::NB_CHANNELS;
        size_t nbSamples = 128;
        size_t maxDepth = 4;
        int nbThreads = 32;
        ImportanceSamplingStrategy strategy = ImportanceSamplingStrategy::LightsAndBSDF;
        IntegratorStrategy integratorStrategy = IntegratorStrategy::PathTracerDirectLighting;
        bool shuffleRandom = true;
    };

public:
    PathTracer(const Parameters& parameters = {});
    ~PathTracer();

public:
    int init();
    bool iterate();
    const float* getImageBuffer() const;
    void setCamera(std::shared_ptr<Camera> camera);
    void setScene(const SceneData& scene);
    bool start();


private:
    glm::vec3 _getRadianceFromCameraRay(const Ray& camera_ray, size_t max_depth) const;
    glm::vec3 _pathTracingDirectLighting(const Ray& r, size_t max_depth = 0) const;
    glm::vec3 _importanceSamplingRadiance(const glm::vec3& wo, const glm::vec3& pathWeight, const HitRecord& surfaceRecord, const ImportanceSamplingStrategy& strategy) const;
    glm::vec3 _simplePathTracing(const Ray& camera_ray, size_t max_depth) const;

private:
    float* _accumulationBuffer = nullptr;
    float* _finalBuffer = nullptr;

    std::shared_ptr<Camera> _camera;

    const SceneData* _scene = nullptr;

    const Parameters _parameters;

    int _currentSample = 1;
    const size_t _portionSize;
};

