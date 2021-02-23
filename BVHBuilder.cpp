#include "BVHBuilder.h"

#include "Utils.h"
#include "AABB.h"
#include "BVHNode.h"

#include <iostream>
#include <algorithm>

namespace {
	struct CustomLess {
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
		float t0 = 0;
		float t1 = std::numeric_limits<float>::max();
	};

	struct SplitInfo {
		SplitInfo(std::shared_ptr<BVHNode> node, size_t offset, size_t size) :
			node(node),
			offset(offset),
			size(size)
		{}

		std::shared_ptr<BVHNode> node;
		size_t offset;
		size_t size;
	};
}

std::shared_ptr<BVHNode> BVHBuilder::buildBVH(std::vector<std::shared_ptr<Hitable>>& list, size_t offset, size_t size, float time0, float time1)
{
	if (size == 0)
		return nullptr;

	std::vector<SplitInfo> building_queue;
	building_queue.push_back(SplitInfo(std::make_shared<BVHNode>(), offset, size));

	std::vector<std::shared_ptr<BVHNode>> bvh_nodes;

	while (!building_queue.empty()) {
		SplitInfo split_info = building_queue.back();
		building_queue.pop_back();
		bvh_nodes.push_back(split_info.node);

		int axis = static_cast<int>(frand() * 3.f);
		CustomLess custom_less;
		custom_less.axis = axis;
		custom_less.t0 = time0;
		custom_less.t1 = time1;
		std::sort(list.begin() + split_info.offset, list.begin() + split_info.offset + split_info.size, custom_less);
		if (split_info.size == 2) {
			split_info.node->left = list[split_info.offset];
			split_info.node->right = list[static_cast<size_t>(split_info.offset) + 1];
		}
		else if (split_info.size == 1) {
			split_info.node->left = split_info.node->right = list[split_info.offset];
		}
		else {
			std::shared_ptr<BVHNode> left = std::make_shared<BVHNode>();
			std::shared_ptr<BVHNode> right = std::make_shared<BVHNode>();
			split_info.node->left = left;
			split_info.node->right = right;
			building_queue.push_back(SplitInfo(right, split_info.offset + split_info.size / 2, split_info.size - split_info.size / 2));
			building_queue.push_back(SplitInfo(left, split_info.offset, split_info.size / 2));
		}
	}

	for (int i = static_cast<int>(bvh_nodes.size()) - 1; i >= 0; --i) {
		AABB box_left, box_right;
		if (!bvh_nodes[i]->left->boundingBox(time0, time1, box_left)) {
			std::cerr << "Hitable a has no bounding box!" << std::endl;
		}
		if (!bvh_nodes[i]->right->boundingBox(time0, time1, box_right)) {
			std::cerr << "Hitable b has no bounding box!" << std::endl;
		}
		bvh_nodes[i]->box = surrounding_box(box_left, box_right);
	}

	return bvh_nodes[0];
}

