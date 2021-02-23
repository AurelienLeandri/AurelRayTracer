#pragma once

#include <memory>

#include <glm.hpp>

#include "Material.h"
#include "Ray.h"
#include "HitRecord.h"

class Material;

class AABB;

class Hitable
{
public:
	Hitable() = default;
	Hitable(std::shared_ptr<Material> material);
	virtual ~Hitable() = default;

public:
	virtual bool hit(Ray r, float t_min, float t_max, HitRecord& record) const = 0;
	virtual bool hit(Ray r, HitRecord &record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const = 0;
	//virtual float pdf_value(const glm::vec3& origin, const glm::vec3& direction) const { return 0.f; }
	virtual glm::vec3 random(const glm::vec3& origin) const { return glm::vec3(1.f, 0.f, 0.f); };  // TODO: make pure virtual
	virtual void transform(const glm::vec3& translation, const glm::vec3& rotation = glm::vec3()) = 0;

public:
	std::shared_ptr<Material> material;
};

