#pragma once

#include <memory>

#include <glm.hpp>

#include "Material.h"
#include "Ray.h"
#include "HitRecord.h"

class Material;

class AABB;

enum class HitableType {
	HITABLE_TYPE_MESH,
	HITABLE_TYPE_SPHERE,
	HITABLE_TYPE_SIZE,
};

class Hitable
{
public:
	Hitable() = default;
	Hitable(std::shared_ptr<Material> material);
	virtual ~Hitable() = default;

public:
	virtual bool hit(Ray r, float t_min, float t_max, HitRecord& record) const { return false; };
	virtual bool hit(Ray r, HitRecord &record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const { return false; };
	virtual float sample(glm::vec3& value, const glm::vec3& origin, const glm::vec3& origin_normal) const { return 0.f; };
	virtual void transform(const glm::vec3& translation, const glm::vec3& rotation = glm::vec3()) {};

public:
	std::shared_ptr<Material> material;
};

