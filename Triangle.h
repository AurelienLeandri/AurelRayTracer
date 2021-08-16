#pragma once

#include "Shape.h"

#include "Vertex.h"

#include <array>

class Triangle : public Shape
{
// Attributes
private:
	std::array<Vertex, 3> _vertices;
	glm::vec3 _normal;
	float _parallelogramArea;

// Constructors/Destructors
public:
	Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2);
private:
	void _computeTriangleNormalAndParallelogramArea();

// Inherited via Shape
public:
	virtual glm::vec3 sample(const HitRecord& record, float& pdf) const override;
	virtual float pdf(const glm::vec3& point, const HitRecord& record) const override;
	virtual void transform(const Transform& t) override;
	virtual float area() const override;

// Accessors
public:
	const std::array<Vertex, 3>& getVertices() const;
	std::array<Vertex, 3>& getVertices();
	const Vertex& operator[](int index) const;
	Vertex& operator[](int index);
};

