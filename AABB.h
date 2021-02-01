#pragma once
#include "Hitable.h"

class AABB :
	public Hitable
{
public:
	AABB();
	AABB(const glm::vec3 &min, const glm::vec3 &max);

	virtual bool hit(Ray r, float t_min, float t_max, HitRecord& record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const;
	virtual void transform(const glm::vec3& translation, const glm::vec3& rotation = glm::vec3()) {}

	const glm::vec3& getMin() const;
	const glm::vec3& getMax() const;

private:
	glm::vec3 _min;
	glm::vec3 _max;
};

AABB surrounding_box(const AABB box0, const AABB box1);
