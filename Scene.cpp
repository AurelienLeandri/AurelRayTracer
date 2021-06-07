#include "Scene.h"

#include "Embree.h"
#include "Shape.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Material.h"

std::vector<SceneData> SceneFactory::_scenes;

namespace { // embree

	void errorFunction(void* userPtr, enum RTCError error, const char* str)
	{
		printf("error %d: %s\n", error, str);
	}

}

SceneData::~SceneData()
{
    if (_rtcScene) {
        rtcReleaseScene(_rtcScene);
    }
    if (_rtcDevice) {
        rtcReleaseDevice(_rtcDevice);
    }
}

bool SceneData::computeAccelerationStructures()
{
	_rtcDevice = rtcNewDevice(nullptr);

	if (!_rtcDevice) {
		printf("error %d: cannot create device\n", rtcGetDeviceError(nullptr));
		return 1;
	}

	rtcSetDeviceErrorFunction(_rtcDevice, errorFunction, nullptr);

	_rtcScene = rtcNewScene(_rtcDevice);

	for (const std::pair<unsigned int, std::shared_ptr<Shape>>& pair : _geometries) {
		pair.second->commitGeometry(_rtcDevice, _rtcScene);
	}

	rtcCommitScene(_rtcScene);

	return true;
}

/*
* Cast a single ray with origin (ox, oy, oz) and direction
* (dx, dy, dz).
*/
bool SceneData::castRay(const Ray& ray, HitRecord& hit_record) const
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

        std::shared_ptr<Shape> shape = _geometries.at(rayhit.hit.geomID);

        hit_record.shapeId = rayhit.hit.geomID;

        // If the shape we hit is a light source, we attach it to the record.
        if (_areaLights.find(hit_record.shapeId) != _areaLights.end())
            hit_record.areaLight = _areaLights.at(hit_record.shapeId);

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
            hit_record.material = _materials.at(material_id).get();
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

const std::unordered_map<unsigned int, std::shared_ptr<Shape>>& SceneData::getShapes() const
{
	return _geometries;
}

std::unordered_map<unsigned int, std::shared_ptr<Shape>>& SceneData::getShapes()
{
	return _geometries;
}

const std::unordered_map<unsigned int, std::shared_ptr<Material>>& SceneData::getMaterials() const
{
	return _materials;
}

std::unordered_map<unsigned int, std::shared_ptr<Material>>& SceneData::getMaterials()
{
	return _materials;
}

const std::vector<std::shared_ptr<const Light>>& SceneData::getLights() const
{
	return _lights;
}

std::vector<std::shared_ptr<const Light>>& SceneData::getLights()
{
	return _lights;
}

unsigned int SceneData::addShape(std::shared_ptr<Shape> shape)
{
	_geometries[_geometries.size()] = shape;
	return _geometries.size() - 1;
}

unsigned int SceneData::addLight(std::shared_ptr<Light> light, std::shared_ptr<Shape> lightShape)
{
	_lights.push_back(light);
	if (lightShape) {
        int shapeId = _geometries.size();
        _geometries[shapeId] = lightShape;
        _areaLights[shapeId] = light;
		return _geometries.size() - 1;
	}
	return 0;
}

unsigned int SceneData::addMaterial(std::shared_ptr<Material> material)
{
	_materials[_materials.size() + 1] = material;
	return _materials.size();
}

SceneData *SceneFactory::createScene()
{
	_scenes.push_back(SceneData());
	return &_scenes.back();
}
