#pragma once
#include "Hitable.h"

#include <vector>
#include <memory>

class HitableList :
    public Hitable
{
public:
    virtual bool hit(Ray r, float t_min, float t_max, HitRecord &record) const;
	virtual bool boundingBox(float t0, float t1, AABB& box) const;
    virtual void transform(const glm::vec3& translation, const glm::vec3& rotation = glm::vec3());

public:
    void concatenateHitableList(std::shared_ptr<HitableList> hitableList);

    std::vector<std::shared_ptr<Hitable>> list;
};

