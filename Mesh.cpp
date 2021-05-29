#include "Mesh.h"

#include "Transform.h"
#include "Embree.h"

Mesh::Mesh()
    : Shape(ShapeType::MESH)
{
}

void Mesh::transform(const Transform& t)
{
	for (Vertex& v : geometry) {
		v.normal = glm::vec3(glm::transpose(t.getInvMatrix()) * glm::vec4(v.normal, 0));
		v.position = glm::vec3(t.getMatrix() * glm::vec4(v.position, 1));
	}
}

float Mesh::area() const
{
	return 0.0f;
}

void Mesh::commitGeometry(RTCDevice device, RTCScene rtcScene)
{
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

    // Vertices
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, &geometry[0], 0, sizeof(Vertex), geometry.size());

    // Normals
    rtcSetGeometryVertexAttributeCount(geom, 1);
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, &geometry[0], sizeof(glm::vec3), sizeof(Vertex), geometry.size());

    // UVs
    rtcSetGeometryVertexAttributeCount(geom, 2);
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT3, &geometry[0], sizeof(glm::vec3) * 2, sizeof(Vertex), geometry.size());

    // Indices
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, &indices[0], 0, 3 * sizeof(unsigned int), indices.size() / 3);

    rtcCommitGeometry(geom);
    rtcAttachGeometry(rtcScene, geom);
    rtcReleaseGeometry(geom);
}

glm::vec3 Mesh::sample(const HitRecord& record, float& pdf) const
{
	pdf = 0;
	return glm::vec3(0, 0, 0);
}

float Mesh::pdf(const glm::vec3& point, const HitRecord& record) const
{
	return 0.0f;
}
