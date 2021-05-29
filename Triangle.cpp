#include "Triangle.h"

#include "Utils.h"
#include "HitRecord.h"
#include "Transform.h"
#include "Embree.h"

Triangle::Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2)
	: Shape(ShapeType::TRIANGLE)
{
	_vertices[0] = vertex0;
	_vertices[1] = vertex1;
	_vertices[2] = vertex2;
	_computeTriangleNormalAndParallelogramArea();
}

const Vertex& Triangle::operator[](int index) const
{
	return _vertices[index];
}

void Triangle::_computeTriangleNormalAndParallelogramArea()
{
	_normal = glm::cross(_vertices[1].position - _vertices[0].position, _vertices[2].position - _vertices[0].position);
	_parallelogramArea = glm::length(_normal);
	_normal = glm::normalize(_normal);
}

glm::vec3 Triangle::sample(const HitRecord& record, float& pdf) const {
	pdf = 0;
	float u = frand();
	float v = frand();
	if (u + v > 1) {
		u = 1 - u;
		v = 1 - v;
	}
	glm::vec3 point_on_triangle = (u * (_vertices[1].position - _vertices[0].position) + v * (_vertices[2].position - _vertices[0].position)) + _vertices[0].position;
	glm::vec3 value = point_on_triangle - record.position;
	float light_cosine = glm::dot(glm::normalize(value), record.normal);
	if (light_cosine < 0.1 || glm::dot(_normal, value) >= 0)
		return value;
	float length_squared = glm::dot(value, value);
	float result = glm::dot(value, value) / (glm::dot(glm::normalize(value), record.normal) * (_parallelogramArea * 0.5f));
	pdf = this->pdf(value, record);
	return value;
}

float Triangle::pdf(const glm::vec3& point, const HitRecord& record) const
{
	return std::fabs(glm::dot(point, point)) / (std::fabs(glm::dot(glm::normalize(point), record.normal)) * (area() * 0.5f));
}

void Triangle::transform(const Transform& t)
{
	for (Vertex& v : _vertices) {
		v.normal = glm::vec3(glm::transpose(t.getInvMatrix()) * glm::vec4(v.normal, 0));
		v.position = glm::vec3(t.getMatrix() * glm::vec4(v.position, 1));
	}
}

float Triangle::area() const
{
	return _parallelogramArea * 0.5f;
}

void Triangle::commitGeometry(RTCDevice device, RTCScene rtcScene)
{
	static int indices[3] = { 0, 1, 2 };
	RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

	// Vertices
	rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, &_vertices[0], 0, sizeof(Vertex), _vertices.size());

	// Normals
	rtcSetGeometryVertexAttributeCount(geom, 1);
	rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, &_vertices[0], sizeof(glm::vec3), sizeof(Vertex), _vertices.size());

	// UVs
	rtcSetGeometryVertexAttributeCount(geom, 2);
	rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT3, &_vertices[0], sizeof(glm::vec3) * 2, sizeof(Vertex), _vertices.size());

	// Indices
	rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, &indices[0], 0, 3 * sizeof(unsigned int), 1 /* <- only 1 triangle */);

	rtcCommitGeometry(geom);
	rtcAttachGeometry(rtcScene, geom);
	rtcReleaseGeometry(geom);
}


