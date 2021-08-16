#include "Mesh.h"

#include "Transform.h"

// Constructors/Destructors

Mesh::Mesh()
    : Shape(Shape::Type::MESH)
{
}

// Inherited vie Shape

glm::vec3 Mesh::sample(const HitRecord& record, float& pdf) const
{
	// TODO: Implement sampling
	pdf = 0;
	return glm::vec3(0, 0, 0);
}

float Mesh::pdf(const glm::vec3& point, const HitRecord& record) const
{
	// TODO: Implement sampling
	return 0.0f;
}

void Mesh::transform(const Transform& t)
{
	for (Vertex& v : _vertices) {
		v.normal = glm::vec3(glm::transpose(t.getInvMatrix()) * glm::vec4(v.normal, 0));
		v.position = glm::vec3(t.getMatrix() * glm::vec4(v.position, 1));
	}
}

float Mesh::area() const
{
	// TODO: Implement computing the area
	return 0.0f;
}

// Accessors

const std::vector<Vertex>& Mesh::getVertices() const
{
	return _vertices;
}

std::vector<Vertex>& Mesh::getVertices()
{
	return _vertices;
}

const std::vector<int>& Mesh::getIndices() const
{
	return _indices;
}

std::vector<int>& Mesh::getIndices()
{
	return _indices;
}

