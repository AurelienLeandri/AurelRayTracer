
#include "Sphere.h"

#include "AABB.h"
#include "Utils.h"

#include <math.h>
#include <algorithm>

Sphere::Sphere(std::shared_ptr<Material> material) : Hitable(material)
{
}

Sphere::Sphere(glm::vec3 center, float radius, std::shared_ptr<Material> material) : Hitable(material), center(center), radius(radius)
{
}

bool Sphere::hit(Ray r, float t_min, float t_max, HitRecord & record) const
{
	float quadratic = dot(r.direction, r.direction);
	float linear = 2 * dot(r.direction, r.origin) - 2 * dot(r.direction, center);
	float constant = dot(r.origin, r.origin) - 2 * dot(r.origin, center) + dot(center, center) - radius * radius;
	float delta = linear * linear - 4 * quadratic * constant;

	if (delta < 0) {
		return false;
	}

	float t = 0.f;
	if (delta == 0) {
		t = -linear / (2 * quadratic);
		if (t < t_min || t >= t_max)
			return false;
	}
	else if (delta >= 0) {
		float sqrt_delta = sqrt(delta);
		float inv_2quadratic = 1.f / (2 * quadratic);
		float t0 = (-linear - sqrt_delta) * inv_2quadratic;
		float t1 = (-linear + sqrt_delta) * inv_2quadratic;
		if (t0 <= t1 && t0 >= t_min && t0 < t_max) {
			t = t0;
		}
		else if (t1 >= t_min && t1 < t_max) {
			t = t1;
		}
		else {
			return false;
		}
	}

	record.t = t;
	record.position = r.pointAtParameter(record.t);
	record.normal = record.position - center;
	record.normal = glm::normalize(record.normal);
	record.material = material.get();
	/*
	if (material) {
		material->getBSDF(record);
		material->emit(record);
	}
	*/

    get_sphere_uv(record.normal, record.u, record.v);

	return true;
}

void Sphere::transform(const glm::vec3& translation, const glm::vec3& rotation)
{
	center += translation;
}

bool Sphere::boundingBox(float t0, float t1, AABB& box) const
{
	box = AABB(center - glm::vec3(radius, radius, radius), center + glm::vec3(radius, radius, radius));
	return true;
}
