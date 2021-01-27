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
	//virtual bool boundingBox(float t0, float t1, AABB& box) const;

    glm::vec3 center;
    float radius = 1.f;
};

