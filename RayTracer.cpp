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
#include "InfiniteAreaLight.h"
#include "Embree.h"
#include "Texture.h"
#include "SceneData.h"
#include "Mesh.h"
#include "Material.h"
#include "Triangle.h"


static float max_green = 0;

RayTracer::~RayTracer()
{
    if (_imageBuffer) {
        delete[] _imageBuffer;
        _imageBuffer = nullptr;
    }
    if (_rtcScene) {
        rtcReleaseScene(_rtcScene);
    }
    if (_rtcDevice) {
        rtcReleaseDevice(_rtcDevice);
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
        if (_castRay(w_o, hit_record)) {

            if (depth == 1) {  // Hitting lights on the first ray
                color += hit_record.emission;
            }

            glm::vec3 w_o_calculations = glm::normalize(-w_o.direction);

            // Direct lighting
            for (unsigned int light_id : _lights) {
                const std::shared_ptr<Shape> &light = _scene->getShapes().at(light_id);
                float light_sample_proba = 0;
                glm::vec3 light_sample = light->sample(hit_record, light_sample_proba);
                HitRecord occlusion_hit_record;
                Ray direct_lighting_ray(hit_record.position, light_sample);
                light_sample = glm::normalize(light_sample);
                // If the light can be sampled from our position, we check if we hit the light:
                // To verify this, "occlusion_hit_record.tRay" should be very close to one since "light_sample" stretches from the current position to the light.
                if (light_sample_proba > 0.000001f && _castRay(direct_lighting_ray, occlusion_hit_record) && occlusion_hit_record.shapeId == light_id) {
                    float cos_light_surface = glm::dot(light_sample, hit_record.normal);
                    if (cos_light_surface > 0) {
                        glm::vec3 light_f = hit_record.bsdf.f(light_sample, w_o_calculations, hit_record);
                        glm::vec3 light_scattering = light_f * cos_light_surface;  // The direct lighing is affected by the surface properties and by the cos factor
                        glm::vec3 light_color = occlusion_hit_record.emission * path_accumulated_weight * light_scattering / light_sample_proba;
                        color += light_color;
                    }
                }
            }

            /*
            static std::shared_ptr<ImageTexture> environment_emission_texture = std::make_shared<ImageTexture>("lakeside_2k.hdr");
            static InfiniteAreaLight light(environment_emission_texture->getData(), environment_emission_texture->getWidth(), environment_emission_texture->getHeight(), environment_emission_texture->getNbChannels());
            glm::vec3 light_sample(0, 0, 0);
            float light_sample_proba = light.sample(light_sample, hit_record.position, hit_record.normal);
            HitRecord occlusion_hit_record;
            Ray direct_lighting_ray(hit_record.position, light_sample);
            light_sample = glm::normalize(light_sample);
            // If the light can be sampled from our position, we check if we hit the light:
            // To verify this, "occlusion_hit_record.tRay" should be very close to one since "light_sample" stretches from the current position to the light.
            bool hit_anything = _castRay(direct_lighting_ray, occlusion_hit_record);
            if (light_sample_proba > 0.f && !hit_anything) {
                float cos_light_surface = glm::dot(light_sample, hit_record.normal);
                if (cos_light_surface > 0) {
                    glm::vec3 light_f = hit_record.bsdf.f(light_sample, w_o_calculations, hit_record);
                    glm::vec3 light_scattering = light_f * cos_light_surface;  // The direct lighing is affected by the surface properties and by the cos factor
                    //glm::vec3 light_color = occlusion_hit_record.emission * path_accumulated_weight * light_scattering / light_sample_proba;
                    glm::vec3 light_color = glm::vec3(1, 1, 1) * path_accumulated_weight * light_scattering / light_sample_proba;
                    color += light_color;
                }
            }
            */

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
            //color += path_accumulated_weight * glm::vec3(1);
            return color;
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
    rayhit.ray.tnear = 0.01f;
    rayhit.ray.tfar = std::numeric_limits<float>::infinity();
    rayhit.ray.mask = -1;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

    /*
     * There are multiple variants of rtcIntersect. This one
     * intersects a single ray with the scene.
     */
    rtcIntersect1(_rtcScene, &context, &rayhit);

    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        hit_record.position = ray.pointAtParameter(rayhit.ray.tfar);
        hit_record.normal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));;
        hit_record.tRay = rayhit.ray.tfar;

        std::shared_ptr<Shape> shape = _scene->getShapes().at(rayhit.hit.geomID);

        hit_record.shapeId = rayhit.hit.geomID;
        
        if (shape->type == ShapeType::TRIANGLE || shape->type == ShapeType::MESH) {
            // Interpolation of vertex data
            static struct alignas (16) interpolated_data {
                float normals[4] = { 0 };
                float uvs[4] = { 0 };
            } interpolation;
            RTCGeometry geometry = rtcGetGeometry(_rtcScene, rayhit.hit.geomID);
            rtcInterpolate1(geometry, rayhit.hit.primID, rayhit.hit.u, rayhit.hit.v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &interpolation.normals[0], nullptr, nullptr, 4);
            rtcInterpolate1(geometry, rayhit.hit.primID, rayhit.hit.u, rayhit.hit.v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &interpolation.uvs[0], nullptr, nullptr, 4);

            hit_record.normal = glm::normalize(glm::vec3(interpolation.normals[0], interpolation.normals[1], interpolation.normals[2]));

            hit_record.u = interpolation.uvs[0];
            hit_record.v = interpolation.uvs[1];
        }
        else if (shape->type == ShapeType::SPHERE) {
            hit_record.normal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));
            hit_record.u = rayhit.hit.u;
            hit_record.v = rayhit.hit.v;
        }

        // Compute shading coordinate system
        if (glm::dot(hit_record.normal, ray.direction) > 0)
            hit_record.normal = -hit_record.normal;
        glm::vec3 a = glm::abs(hit_record.normal.x) > 0.9f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
        hit_record.bitangent = glm::normalize(glm::cross(a, hit_record.normal));
        hit_record.tangent = glm::cross(hit_record.normal, hit_record.bitangent);

        hit_record.shadingCoordinateSystem[0] = glm::vec3(hit_record.tangent.x, hit_record.bitangent.x, hit_record.normal.x);
        hit_record.shadingCoordinateSystem[1] = glm::vec3(hit_record.tangent.y, hit_record.bitangent.y, hit_record.normal.y);
        hit_record.shadingCoordinateSystem[2] = glm::vec3(hit_record.tangent.z, hit_record.bitangent.z, hit_record.normal.z);

        hit_record.ray = ray;

        if (unsigned int material_id = shape->materialId) {
            hit_record.material = _scene->getMaterials().at(material_id).get();
            hit_record.material->getBSDF(hit_record);
            hit_record.material->emit(hit_record);
        }
        else {  // Default material
            static MatteMaterial default_material(ConstantTexture::white);
            hit_record.material = &default_material;
            default_material.getBSDF(hit_record);
            default_material.emit(hit_record);
        }

        return true;
    }
    return false;
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

    // Register meshes with emissive material as lights, to retrieve them by index in the scene geometry for importance sampling
    for (const std::pair<unsigned int, std::shared_ptr<Shape>>& pair : _scene->getShapes()) {
        if (pair.second->materialId) {
            std::shared_ptr<Material> mesh_material = _scene->getMaterials().at(pair.second->materialId);
            if (pair.second->materialId && (dynamic_cast<const EmissiveMaterial*>(mesh_material.get()))) {  // Light source TODO: better check if a material is emissive
                _lights.push_back(pair.first);
            }
        }
    }

    _rtcDevice = rtcNewDevice(nullptr);

    if (!_rtcDevice) {
        printf("error %d: cannot create device\n", rtcGetDeviceError(nullptr));
        return 1;
    }

    rtcSetDeviceErrorFunction(_rtcDevice, errorFunction, nullptr);

    _rtcScene = rtcNewScene(_rtcDevice);

    for (const std::pair<unsigned int, std::shared_ptr<Shape>>& pair : _scene->getShapes()) {
        pair.second->commitGeometry(_rtcDevice, _rtcScene);
    }
    
    rtcCommitScene(_rtcScene);

    return true;
}

