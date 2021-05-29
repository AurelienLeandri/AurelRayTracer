#pragma once

#include "Shape.h"

#include "Vertex.h"

#include <array>

class Triangle : public Shape
{
public:
	Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2);

public:
	// Inherited via Shape
	virtual glm::vec3 sample(const HitRecord& record, float& pdf) const override;
	virtual float pdf(const glm::vec3& point, const HitRecord& record) const override;
	virtual void transform(const Transform& t) override;
	virtual float area() const override;
	virtual void commitGeometry(RTCDevice device, RTCScene rtcScene) override;

private:
	void _computeTriangleNormalAndParallelogramArea();

private:
	std::array<Vertex, 3> _vertices;
	const Vertex& operator[](int index) const;
	float _parallelogramArea = 0;
	glm::vec3 _normal;
};

