#pragma once
#include "Hitable.h"

#include <vector>
#include <memory>

class HitableList :
    public Hitable
{
public:
    virtual bool hit(Ray r, float t_min, float t_max, HitRecord &record) const;
	//virtual bool boundingBox(float t0, float t1, AABB& box) const;

    std::vector<std::shared_ptr<Hitable>> list;
};

