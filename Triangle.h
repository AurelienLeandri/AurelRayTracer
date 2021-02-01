#pragma once

#include "Hitable.h"

#include "Vertex.h"

#include <memory>

class Triangle : public Hitable
{
public:
	Triangle(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, std::shared_ptr<Material> material = nullptr,
		const glm::vec3& translation = glm::vec3(), const glm::vec3& rotation = glm::vec3());

public:
	// Inherited via Hitable
	virtual bool hit(Ray r, float t_min, float t_max, HitRecord& record) const override;
	virtual bool boundingBox(float t0, float t1, AABB& box) const override;
	virtual void transform(const glm::vec3& translation, const glm::vec3& rotation = glm::vec3());

public:
	void _computeTriangleNormal();
	void _computePlaneDParameter();
	bool _oldAlgo(Ray r, float t_min, float t_max, HitRecord& record) const;

private:
	Vertex _vertices[3];

public:
	const Vertex& v0 = _vertices[0];
	const Vertex& v1 = _vertices[1];
	const Vertex& v2 = _vertices[2];
	const Vertex& operator[](int index) const;

private:
	// Normal of the triangle and D parameter of the triangle plane for faster hit function
	glm::vec3 _normal = glm::vec3(1, 0, 0);
	float _D = 0;
};

