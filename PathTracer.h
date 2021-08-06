#pragma once

#include <glm/glm.hpp>
#include <omp.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <embree3/rtcore.h>

#include "ApplicationOld.h"
#include "Vertex.h"
#include "BxDF.h"
#include "Integrator.h"

class Camera;
class Ray;
struct HitRecord;
class Material;
class Scene;
class Light;

class PathTracer : public Integrator
{
private:
    enum DirectLightingSamplingStrategy {
        LightsOnly = 1 << 0,
        BSDFOnly = 1 << 1,
        LightsAndBSDF = 1 << 2
    };

public:
    struct Parameters {
        size_t width = ApplicationOld::WIDTH;
        size_t height = ApplicationOld::HEIGHT;
        size_t nbChannels = ApplicationOld::NB_CHANNELS;
        size_t nbSamples = 2048;
        int nbThreads = 32;
    };

public:
    PathTracer(const Parameters& parameters = {});

public:
    virtual int init();
    virtual bool iterate();
    virtual int cleanup();

    const float* getImageBuffer() const;
    void setCamera(std::shared_ptr<Camera> camera);
    void setScene(const Scene& scene);
    glm::vec3 _directLighting(const glm::vec3& wo, const glm::vec3& pathWeight, const HitRecord& surfaceRecord, const DirectLightingSamplingStrategy& strategy = DirectLightingSamplingStrategy::LightsAndBSDF) const;


private:
    glm::vec3 _getColor(const Ray& r, size_t max_depth = 0) const;

private:
    const Parameters _parameters;

    unsigned int _currentSample = 1;
    const size_t _portionSize;
};

