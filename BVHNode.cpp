
#include "BVHNode.h"

#include "Utils.h"

#include <iostream>
#include <algorithm>

BVHNode::BVHNode()
{
}


bool BVHNode::hit(Ray r, float t_min, float t_max, HitRecord& record) const
{
	if (box.hit(r, t_min, t_max, record)) {
		HitRecord left_record;
		HitRecord right_record;
		bool hit_left = left->hit(r, t_min, t_max, left_record);
		bool hit_right = right->hit(r, t_min, t_max, right_record);
		if (hit_left && hit_right) {
			record = left_record.tRay < right_record.tRay ? left_record : right_record;
			return true;
		}
		else if (hit_left) {
			record = left_record;
			return true;
		}
		else if (hit_right) {
			record = right_record;
			return true;
		}
	}
	return false;
}

bool BVHNode::boundingBox(float t0, float t1, AABB& box) const
{
	box = this->box;
	return true;
}
