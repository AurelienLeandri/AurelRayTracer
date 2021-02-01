
#include "AABB.h"

#include <algorithm>

namespace {

	float ffmin(float a, float b) {
		return a < b ? a : b;
	}

	float ffmax(float a, float b) {
		return a > b ? a : b;
	}

}

AABB::AABB()
{
}

AABB::AABB(const glm::vec3& min, const glm::vec3& max) : _min(min), _max(max)
{
}

bool AABB::hit(Ray r, float t_min, float t_max, HitRecord& record) const
{
	/*
	p(t0x) = x0 = Ax * t0x + Bx
	x0 = Ax * tx0 + Bx
	t0x = (x0 - Bx) / Ax
	*/
	float t0 = 0.f;
	float t1 = 0.f;
	float invD = 0.f;
	for (int i = 0; i < 3; ++i) {
		// Case origin origin.components[i] close to 0: Both t0 and t1 will be either both +inf of both -inf, so this is safe (comparisons will work)
		// We have to check later for NaN (case numerator and denominator are 0)
		invD = 1.f / r.direction[i];
		t0 = (_min[i] - r.origin[i]) * invD;
		t1 = (_max[i] - r.origin[i]) * invD;
		if (invD < 0)
			std::swap(t0, t1);
		t_min = ffmax(t_min, t0);
		t_max = ffmin(t_max, t1);
		if (t_max <= t_min)
			return false;
	}
	return true;
}

bool AABB::boundingBox(float t0, float t1, AABB& box) const
{
	box = *this;
	return true;
}

const glm::vec3& AABB::getMin() const
{
	return _min;
}

const glm::vec3& AABB::getMax() const
{
	return _max;
}

AABB surrounding_box(const AABB box0, const AABB box1)
{
	glm::vec3 min_vec = glm::vec3(
		ffmin(box0.getMin().x, box1.getMin().x),
		ffmin(box0.getMin().y, box1.getMin().y),
		ffmin(box0.getMin().z, box1.getMin().z)
	);
	glm::vec3 max_vec = glm::vec3(
		ffmax(box0.getMax().x, box1.getMax().x),
		ffmax(box0.getMax().y, box1.getMax().y),
		ffmax(box0.getMax().z, box1.getMax().z)
	);
	return AABB(min_vec, max_vec);
}
