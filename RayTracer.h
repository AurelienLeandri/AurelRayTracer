#pragma once

#include <glm.hpp>
#include <omp.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <embree3/rtcore.h>

#include "Application.h"
#include "Vertex.h"

class Hitable;
class Camera;
class Ray;
class HitRecord;
class Mesh;
class Material;

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
    void setScene(const std::unordered_map<unsigned int, std::shared_ptr<Mesh>>& meshes, const std::unordered_map<unsigned int, std::shared_ptr<Material>>& materials);
    bool start();


private:
    glm::vec3 _getColor(const Ray& r) const;
    bool _hit(Ray r, float t_min, float t_max, HitRecord& record) const;
    bool _hit(Ray r, HitRecord& record) const;
    bool _castRay(const Ray& ray, HitRecord& hit_record) const;

private:
    // 16 bytes aligned structures for buffers in embree
    struct alignas(16) _VertexData {
        float x;
        float y;
        float z;
        float _padding = 0;
    };

    struct alignas(16) _UVData {
        float u;
        float v;
        float _padding0 = 0;
        float _padding1 = 0;
    };

    struct alignas(16) _NormalsData {
        float x;
        float y;
        float z;
        float _padding = 0;
    };

    float* _imageBuffer = nullptr;

    const size_t _WIDTH = Application::WIDTH;
    const size_t _HEIGHT = Application::HEIGHT;
    const size_t _NB_CHANNELS = Application::NB_CHANNELS;

    const size_t _NB_SAMPLES = 64;
    const float _INV_NB_SAMPLES = 1.f / _NB_SAMPLES;
    const size_t _NB_THREADS = omp_get_max_threads();
    const size_t _PORTION_SIZE = (_WIDTH * _HEIGHT) / _NB_THREADS;

    std::shared_ptr<Camera> _camera;
    std::shared_ptr<Hitable> _world;

    std::vector<std::shared_ptr<Hitable>> _legacyLights;
    std::unordered_map<unsigned int, std::shared_ptr<Mesh>> _meshes;
    std::unordered_map<unsigned int, std::shared_ptr<Mesh>> _lights;
    std::unordered_map<unsigned int, std::shared_ptr<Material>> _materials;

    std::vector<_VertexData> _vertices;
    std::vector<_NormalsData> _normals;
    std::vector<_UVData> _uvs;
    std::vector<unsigned int> _indices;

    RTCScene _scene = 0;
    RTCDevice _device = 0;

    unsigned int _currentSample = 1;
};

