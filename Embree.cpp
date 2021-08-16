#include "Embree.h"

#include "Scene.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Material.h"
#include "Mesh.h"
#include "Triangle.h"
#include "Sphere.h"

Embree* Embree::_instance = nullptr;

namespace {
    // Error callback for embree
    void errorFunction(void* userPtr, enum RTCError error, const char* str);
}

// Constructors/Destructors

Embree::Embree() :
	_rtcDevice(rtcNewDevice(nullptr))
{
}

Embree::~Embree()
{
	_cleanupScene();
	if (_rtcDevice) {
		rtcReleaseDevice(_rtcDevice);
		_rtcDevice = 0;
	}
}

void Embree::_cleanupScene()
{
    rtcReleaseScene(_rtcScene);
    _rtcScene = 0;
    _scene = nullptr;
}

// Singleton

Embree* Embree::getInstance()
{
	if (_instance)
		return _instance;
	return _instance = new Embree();
}

// Scene setup

void Embree::setScene(const Scene& scene) {
    if (_rtcScene) {
        _cleanupScene();
    }
    _rtcScene = rtcNewScene(_rtcDevice);
    _scene = &scene;

    rtcSetDeviceErrorFunction(_rtcDevice, errorFunction, nullptr);

    _rtcScene = rtcNewScene(_rtcDevice);

    for (const std::pair<int, std::shared_ptr<Shape>>& pair : scene.getShapes()) {
        _addShape(pair.second.get());
    }

    rtcCommitScene(_rtcScene);
}

void Embree::_addShape(const Shape* shape)
{
    switch (shape->type) {
    case Shape::Type::MESH:
        _addMesh(static_cast<const Mesh*>(shape));
        break;
    case Shape::Type::SPHERE:
        _addSphere(static_cast<const Sphere*>(shape));
        break;
    case Shape::Type::TRIANGLE:
        _addTriangle(static_cast<const Triangle*>(shape));
        break;
    default:
        break;
    }
}

void Embree::_addTriangle(const Triangle* triangle)
{
    static const int indices[3] = { 0, 1, 2 };
    RTCGeometry geom = rtcNewGeometry(_rtcDevice, RTC_GEOMETRY_TYPE_TRIANGLE);

    const std::array<Vertex, 3>& vertices = triangle->getVertices();

    // Vertices
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, vertices.data(), 0, sizeof(Vertex), vertices.size());

    // Normals
    rtcSetGeometryVertexAttributeCount(geom, 1);
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, vertices.data(), sizeof(glm::vec3), sizeof(Vertex), vertices.size());

    // UVs
    rtcSetGeometryVertexAttributeCount(geom, 2);
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT3, vertices.data(), sizeof(glm::vec3) * 2, sizeof(Vertex), vertices.size());

    // Indices
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, &indices[0], 0, 3 * sizeof(int), 1 /* <- only 1 triangle */);

    rtcCommitGeometry(geom);
    rtcAttachGeometry(_rtcScene, geom);
    rtcReleaseGeometry(geom);
}

void Embree::_addSphere(const Sphere* sphere)
{
    RTCGeometry geom = rtcNewGeometry(_rtcDevice, RTC_GEOMETRY_TYPE_SPHERE_POINT);
    Sphere::Data* bufferData = (Sphere::Data*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, sizeof(Sphere::Data), 1);

    bufferData->center = sphere->center;
    bufferData->radius = sphere->radius;

    rtcCommitGeometry(geom);
    rtcAttachGeometry(_rtcScene, geom);
    rtcReleaseGeometry(geom);
}

void Embree::_addMesh(const Mesh* mesh)
{
    RTCGeometry geom = rtcNewGeometry(_rtcDevice, RTC_GEOMETRY_TYPE_TRIANGLE);

    const std::vector<Vertex> vertices = mesh->getVertices();
    const std::vector<int> indices = mesh->getIndices();

    // Vertices
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, vertices.data(), 0, sizeof(Vertex), vertices.size());

    // Normals
    rtcSetGeometryVertexAttributeCount(geom, 1);
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, vertices.data(), sizeof(glm::vec3), sizeof(Vertex), vertices.size());

    // UVs
    rtcSetGeometryVertexAttributeCount(geom, 2);
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT3, vertices.data(), sizeof(glm::vec3) * 2, sizeof(Vertex), vertices.size());

    // Indices
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, indices.data(), 0, 3 * sizeof(int), indices.size() / 3);

    rtcCommitGeometry(geom);
    rtcAttachGeometry(_rtcScene, geom);
    rtcReleaseGeometry(geom);
}

// Intersection

bool Embree::castRay(const Ray& ray, HitRecord& hit_record) const
{
    struct RTCIntersectContext context;
    rtcInitIntersectContext(&context);

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

    rtcIntersect1(_rtcScene, &context, &rayhit);

    if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
    {
        hit_record.position = ray.pointAtParameter(rayhit.ray.tfar);
        hit_record.normal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));;
        hit_record.tRay = rayhit.ray.tfar;

        std::shared_ptr<Shape> shape = _scene->getShapes().at(rayhit.hit.geomID);

        hit_record.shapeId = rayhit.hit.geomID;

        // If the shape we hit is a light source, we attach it to the record.
        const std::unordered_map<int, std::shared_ptr<const Light>>& areaLights = _scene->getAreaLights();
        if (areaLights.find(hit_record.shapeId) != areaLights.end())
            hit_record.areaLight = areaLights.at(hit_record.shapeId);

        if (shape->type == Shape::Type::TRIANGLE || shape->type == Shape::Type::MESH) {
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
        else if (shape->type == Shape::Type::SPHERE) {
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

        if (int material_id = shape->getMaterialId()) {
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

namespace {
	void errorFunction(void* userPtr, enum RTCError error, const char* str)
	{
		printf("error %d: %s\n", error, str);
	}
}