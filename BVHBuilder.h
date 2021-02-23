#pragma once

#include <memory>
#include <vector>

class BVHNode;
class Hitable;

class BVHBuilder
{
public:
	static std::shared_ptr<BVHNode> buildBVH(std::vector<std::shared_ptr<Hitable>>& list, size_t offset, size_t size, float time0, float time1);
};

