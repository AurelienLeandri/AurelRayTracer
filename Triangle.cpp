#include "Triangle.h"

#include "Utils.h"

#include <iostream>

Triangle::Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2) : Mesh()
{
	geometry.push_back(vertex0);
	geometry.push_back(vertex1);
	geometry.push_back(vertex2);
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	_computeTriangleNormalAndParallelogramArea();
}

const Vertex& Triangle::operator[](int index) const
{
	return geometry[index];
}

void Triangle::_computeTriangleNormalAndParallelogramArea()
{
	_normal = glm::cross(geometry[1].position - geometry[0].position, geometry[2].position - geometry[0].position);
	_parallelogramArea = glm::length(_normal);
	_normal = glm::normalize(_normal);
}

float Triangle::sample(glm::vec3& value, const glm::vec3& origin, const glm::vec3& origin_normal) const {
	float u = frand();
	float v = frand();
	if (u + v > 1) {
		u = 1 - u;
		v = 1 - v;
	}
	glm::vec3 point_on_triangle = (u * (geometry[1].position - geometry[0].position) + v * (geometry[2].position - geometry[0].position)) + geometry[0].position;
	value = point_on_triangle - origin;
	float light_cosine = glm::dot(glm::normalize(value), origin_normal);
	if (light_cosine < 0.1 || glm::dot(_normal, value) >= 0)
		return 0;
	float length_squared = glm::dot(value, value);
	float result = glm::dot(value, value) / (glm::dot(glm::normalize(value), origin_normal) * (_parallelogramArea * 0.5f));
	return pdf(value, origin, origin_normal);
}

float Triangle::area() const
{
	return _parallelogramArea * 0.5f;
}


