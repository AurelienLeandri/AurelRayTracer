#pragma once

#include "Mesh.h"

#include "Vertex.h"

#include <memory>

class Triangle : public Mesh
{
public:
	Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2);

public:
	virtual float sample(glm::vec3 &value, const glm::vec3& origin, const glm::vec3& origin_normal) const override;
	virtual float area() const;

private:
	void _computeTriangleNormalAndParallelogramArea();

public:
	const Vertex& operator[](int index) const;
	float _parallelogramArea = 0;
	glm::vec3 _normal;
};

