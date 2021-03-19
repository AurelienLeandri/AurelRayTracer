#include "RayTracer.h"

#include <glm.hpp>
#include <iostream>
#include <ctime>
#include <unordered_map>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Ray.h"
#include "Camera.h"
#include "Utils.h"
#include "HitRecord.h"
#include "HitableList.h"
#include "Embree.h"
#include "ImageTexture.h"
#include "Mesh.h"


static float max_green = 0;

RayTracer::~RayTracer()
{
    if (_imageBuffer) {
        delete[] _imageBuffer;
        _imageBuffer = nullptr;
    }
    if (_scene) {
        rtcReleaseScene(_scene);
    }
    if (_device) {
        rtcReleaseDevice(_device);
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
            glm::vec3 sample_color = glm::max(glm::vec3(0, 0, 0), _getColor(r));
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

bool RayTracer::_hit(Ray r, float t_min, float t_max, HitRecord& record) const {
    Embree* embree = nullptr;
    if (embree) {

    }
    else {
        return _world->hit(r, t_min, t_max, record);
    }
}

bool RayTracer::_hit(Ray r, HitRecord& record) const {
    return _hit(r, 0.001f, std::numeric_limits<float>::max(), record);
}

    /*
    * Cast a single ray with origin (ox, oy, oz) and direction
    * (dx, dy, dz).
    */
bool RayTracer::_castRay(const Ray& ray, HitRecord& hit_record) const
{
    /*
     * The intersect context can be used to set intersection
     * filters or flags, and it also contains the instance ID stack
     * used in multi-level instancing.
     */
    struct RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    /*
     * The ray hit structure holds both the ray and the hit.
     * The user must initialize it properly -- see API documentation
     * for rtcIntersect1() for details.
     */
    struct RTCRayHit rayhit;
    rayhit.ray.org_x = ray.origin.x;
    rayhit.ray.org_y = ray.origin.y;
    rayhit.ray.org_z = ray.origin.z;
    rayhit.ray.dir_x = ray.direction.x;
    rayhit.ray.dir_y = ray.direction.y;
    rayhit.ray.dir_z = ray.direction.z;
    rayhit.ray.tnear = 0;
    rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    rayhit.ray.mask = -1;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    /*
     * There are multiple variants of rtcIntersect. This one
     * intersects a single ray with the scene.
     */
    rtcIntersect1(_scene, &context, &rayhit);

    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        hit_record.position = ray.pointAtParameter(rayhit.ray.tfar);
        hit_record.normal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));;
        hit_record.t = rayhit.ray.tfar;

        struct alignas (16) interpolated_data {
            float normals[4] = { 0 };
            float uvs[4] = { 0 };
        };
        interpolated_data interpolation;
        RTCGeometry geometry = rtcGetGeometry(_scene, rayhit.hit.geomID);
        rtcInterpolate1(geometry, rayhit.hit.primID, rayhit.hit.u, rayhit.hit.v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &interpolation.normals[0], nullptr, nullptr, 4);
        rtcInterpolate1(geometry, rayhit.hit.primID, rayhit.hit.u, rayhit.hit.v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &interpolation.uvs[0], nullptr, nullptr, 4);
        hit_record.normal = glm::normalize(glm::vec3(interpolation.normals[0], interpolation.normals[1], interpolation.normals[2]));
        hit_record.u = interpolation.uvs[0];
        hit_record.v = interpolation.uvs[1];

        if (unsigned int material_id = _meshes.at(rayhit.hit.geomID)->materialId) {
            hit_record.material = _materials.at(material_id).get();
            hit_record.material->getBSDF(hit_record);
            hit_record.material->emit(hit_record);
        }
        else {  // Default material
            static Material default_material;
            hit_record.material = &default_material;
            default_material.getBSDF(hit_record);
            default_material.emit(hit_record);
        }

        return true;
    }
    return false;
}

glm::vec3 RayTracer::_getColor(const Ray& camera_ray) const {
    size_t depth = 0;
    glm::vec3 color(0, 0, 0);
    glm::vec3 path_accumulated_weight(1, 1, 1);
    Ray w_o = camera_ray;
    float russian_roulette_weight = 1.f;
        while (true) {
        HitRecord hit_record;
        //if (_world->hit(w_o, hit_record)) {
        if (_castRay(camera_ray, hit_record)) {

            if (depth == 0) {  // Hitting lights on the first ray
                color += hit_record.emission;
            }

            // Direct lighting
            for (const std::pair<unsigned int, std::shared_ptr<Mesh>>& pair : _lights) {
                const std::shared_ptr<Mesh> &light = pair.second;
                glm::vec3 light_sample(0, 0, 0);
                float light_sample_proba = light->sample(light_sample, hit_record.position, hit_record.normal);
                HitRecord occlusion_hit_record;
                // If the light can be sampled from our position, we check if we hit the light:
                // To verify this, "occlusion_hit_record.t" should be very close to one since "light_sample" stretches from the current position to the light.
                if (light_sample_proba > 0 && _castRay(Ray(hit_record.position, light_sample), occlusion_hit_record) && occlusion_hit_record.t > 0.9999f) {
                    float cos_light_surface = glm::dot(glm::normalize(light_sample), hit_record.normal);
                    if (cos_light_surface > 0) {
                        glm::vec3 light_f = hit_record.bsdf->f(light_sample, -w_o.direction, hit_record);
                        glm::vec3 light_scattering = light_f * cos_light_surface;  // The direct lighing is affected by the surface properties and by the cos factor
                        glm::vec3 light_color = occlusion_hit_record.emission * path_accumulated_weight * light_scattering / light_sample_proba;
                        color += light_color;
                    }
                }
            }

            // Computing the next step of the path and updating the accumulated weight
            glm::vec3 w_i(0, 0, 0);
            glm::vec3 sample_proba = hit_record.bsdf->sample_f(w_i, -w_o.direction, hit_record);  // Get a sample vector, gets the proba to pick it
            glm::vec3 f = hit_record.bsdf->f(w_i, -w_o.direction, hit_record);  // Scattering weight from BSDF
            float light_attenuation_wrt_angle = std::fabs(glm::dot(glm::normalize(w_i), hit_record.normal));
            glm::vec3 scattering = light_attenuation_wrt_angle * f;

            if (depth >= 5) {
                russian_roulette_weight = 1.f - glm::min(0.0625f * depth, 1.f);
                if (frand() > russian_roulette_weight) {
                    break;
                }
            }

            glm::vec3 previous_weight = path_accumulated_weight;
            path_accumulated_weight *= scattering / sample_proba;
            path_accumulated_weight /= russian_roulette_weight;
            w_o = Ray(hit_record.position, w_i);
        }
        else {
            return glm::vec3();
            static std::shared_ptr<ImageTexture> environment_emission_texture = std::make_shared<ImageTexture>("lakeside_2k.hdr");
            float u = 0, v = 0;
            get_sphere_uv(glm::normalize(w_o.direction), u, v);
            color += path_accumulated_weight * environment_emission_texture->color(u, v, glm::vec3());
            break;  // The path is over. The ray didn't hit anything
        }

        depth++;
    }
    return color;
}

const float* RayTracer::getImageBuffer() const
{
    return _imageBuffer;
}

void RayTracer::setWorld(const std::shared_ptr<Hitable> world, const std::vector<std::shared_ptr<Hitable>>& lights) {
    _world = world;
    _legacyLights = lights;
}

void RayTracer::setCamera(std::shared_ptr<Camera> camera) {
    this->_camera = camera;
}

void RayTracer::setScene(const std::unordered_map<unsigned int, std::shared_ptr<Mesh>>& meshes, const std::unordered_map<unsigned int, std::shared_ptr<Material>>& materials) {
    _meshes = meshes;
    _materials = materials;
    for (const std::pair<unsigned int, std::shared_ptr<Mesh>>& pair : meshes) {
        if (pair.second->materialId && (materials.at(pair.second->materialId)->emission || materials.at(pair.second->materialId)->emissionValue != glm::vec3(0, 0, 0))) {  // Light source
            _lights[pair.first] = pair.second;
        }
    }
}

namespace { // embree

    void errorFunction(void* userPtr, enum RTCError error, const char* str)
    {
        printf("error %d: %s\n", error, str);
    }

}

bool RayTracer::start() {

    _device = rtcNewDevice(nullptr);

    if (!_device) {
        printf("error %d: cannot create device\n", rtcGetDeviceError(nullptr));
        return 1;
    }

    rtcSetDeviceErrorFunction(_device, errorFunction, nullptr);

    _scene = rtcNewScene(_device);

    for (const std::pair<unsigned int, std::shared_ptr<Mesh>>& pair : _meshes) {
        RTCGeometry geom = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_TRIANGLE);
        for (const Vertex& vertex : pair.second->geometry) {
            _vertices.push_back({ vertex.position.x, vertex.position.y, vertex.position.z });
            _normals.push_back({ vertex.normal.x, vertex.normal.y, vertex.normal.z });
            _uvs.push_back({ vertex.uv.x, vertex.uv.y });
        }
        for (const unsigned int& index : pair.second->indices) {
            _indices.push_back(index);
        }

        // Vertices
        rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, &_vertices[0], 0, sizeof(_VertexData), pair.second->geometry.size());

        // Normals
        rtcSetGeometryVertexAttributeCount(geom, 1);      
        rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, &_normals[0], 0, sizeof(_NormalsData), pair.second->geometry.size());

        // UVs
        rtcSetGeometryVertexAttributeCount(geom, 2);
        rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT3, &_uvs[0], 0, sizeof(_UVData), pair.second->geometry.size());

        // Indices
        rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, &_indices[0], 0, 3 * sizeof(unsigned int), pair.second->indices.size() / 3);

        rtcCommitGeometry(geom);
        rtcAttachGeometry(_scene, geom);
        rtcReleaseGeometry(geom);
    }
    
    rtcCommitScene(_scene);
}

