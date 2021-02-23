#pragma once
#include "Hitable.h"

#include <memory>

class Sphere :
    public Hitable
{
public:
    Sphere(std::shared_ptr<Material> material = nullptr);
    Sphere(glm::vec3 center, float radius, std::shared_ptr<Material> material = nullptr);

    virtual bool hit(Ray r, float t_min, float t_max, HitRecord &record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const;
    virtual void transform(const glm::vec3& translation, const glm::vec3& rotation = glm::vec3());
    virtual glm::vec3 random(const glm::vec3& origin) const override;

    glm::vec3 center = glm::vec3(0, 0, 0);
    float radius = 1.f;
};

