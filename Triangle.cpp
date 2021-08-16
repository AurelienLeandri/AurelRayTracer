#include "Triangle.h"

#include "Utils.h"
#include "HitRecord.h"
#include "Transform.h"
#include "Embree.h"

// Constructors/Destructors

Triangle::Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2)
	: Shape(Shape::Type::TRIANGLE)
{
	_vertices[0] = vertex0;
	_vertices[1] = vertex1;
	_vertices[2] = vertex2;
	_computeTriangleNormalAndParallelogramArea();
}

void Triangle::_computeTriangleNormalAndParallelogramArea()
{
	_normal = glm::cross(_vertices[1].position - _vertices[0].position, _vertices[2].position - _vertices[0].position);
	_parallelogramArea = glm::length(_normal);
	_normal = glm::normalize(_normal);
}

// Inherited from Shape

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

// Accessors

const std::array<Vertex, 3>& Triangle::getVertices() const
{
	return _vertices;
}

std::array<Vertex, 3>& Triangle::getVertices()
{
	return _vertices;
}

const Vertex& Triangle::operator[](int index) const
{
	return _vertices[index];
}

Vertex& Triangle::operator[](int index)
{
	return _vertices[index];
}


