
#include "BVHNode.h"

#include "Utils.h"

#include <iostream>
#include <algorithm>

BVHNode::BVHNode()
{
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Hitable>> &list, int offset, int size, float time0, float time1)
{
	int axis = int(frand() * 3.f);
	static struct {
		bool operator()(std::shared_ptr<Hitable> a, std::shared_ptr<Hitable> b) {
			AABB box_a, box_b;
			if (!a->boundingBox(this->t0, this->t1, box_a)) {
				std::cerr << "Hitable a has no bounding box!" << std::endl;
			}
			if (!b->boundingBox(this->t0, this->t1, box_b)) {
				std::cerr << "Hitable b has no bounding box!" << std::endl;
			}
			if (((box_a.getMin() + box_a.getMax()) / 2.f)[this->axis] - ((box_b.getMin() + box_b.getMax()) / 2.f)[this->axis] < 0.f) {
				return true;
			}
			return false;
		}
		int axis = 0;
		float t0 = t0;
		float t1 = t0;
	} custom_less;
	custom_less.axis = axis;
	custom_less.t0 = time0;
	custom_less.t1 = time1;
	std::sort(list.begin() + offset, list.begin() + offset + size, custom_less);
	if (size == 2) {
		left = list[offset];
		right = list[offset + 1];
	}
	else if (size == 1) {
		left = right = list[offset];
	}
	else {
		left = std::make_shared<BVHNode>(list, offset, size / 2, time0, time1);
		right = std::make_shared<BVHNode>(list, offset + size / 2, size - size / 2, time0, time1);
	}
	AABB box_left, box_right;
	if (!left->boundingBox(time0, time1, box_left)) {
		std::cerr << "Hitable a has no bounding box!" << std::endl;
	}
	if (!right->boundingBox(time0, time1, box_right)) {
		std::cerr << "Hitable b has no bounding box!" << std::endl;
	}
	box = surrounding_box(box_left, box_right);
}

bool BVHNode::hit(Ray r, float t_min, float t_max, HitRecord& record) const
{
	if (box.hit(r, t_min, t_max, record)) {
		HitRecord left_record;
		HitRecord right_record;
		bool hit_left = left->hit(r, t_min, t_max, left_record);
		bool hit_right = right->hit(r, t_min, t_max, right_record);
		if (hit_left && hit_right) {
			record = left_record.t < right_record.t ? left_record : right_record;
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
