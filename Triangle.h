#pragma once

#include "Shape.h"

#include <array>

#include "Vertex.h"

/**
 * Representation of a triangle.
 */
class Triangle : public Shape
{
// Attributes
private:
	/**
	 * Vertices of the triangle.
	 */
	std::array<Vertex, 3> _vertices;

	/**
	 * Normal of the triangle.
	 */
	glm::vec3 _normal;

	/**
	 * Area of the parallelogram spanned by the triangle.
	 */
	float _parallelogramArea;

// Constructors/Destructors
public:
	/**
	 * Creates a triangle from a set of three vertices.
	 */
	Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2);
private:
	/**
	 * Computes the value of the attribute _parallelogramArea as well as the _normal. To be called whenever the vertices are changed.
	 */
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
	void setVertices(const std::array<Vertex, 3>& vertices);
	const Vertex& operator[](int index) const;
};

