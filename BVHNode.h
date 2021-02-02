#pragma once
#include "Hitable.h"

#include "AABB.h"

#include <vector>
#include <memory>

class BVHNode :
	public Hitable
{
public:
	BVHNode();
	virtual bool hit(Ray r, float t_min, float t_max, HitRecord& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const;
	virtual void transform(const glm::vec3& translation, const glm::vec3& rotation = glm::vec3()) {};

	std::shared_ptr<Hitable> left;
	std::shared_ptr<Hitable> right;
	AABB box;
};

