
#include "HitableList.h"

#include "AABB.h"

bool HitableList::hit(Ray r, float t_min, float t_max, HitRecord & record) const
{
    float max_t = t_max;
    bool hit_anything = false;
    for (const auto &hitable : list) {
	  HitRecord candidate_HitRecord;
      if (hitable->hit(r, t_min, max_t, candidate_HitRecord)) {
        hit_anything = true;
        record = candidate_HitRecord;
        max_t = candidate_HitRecord.t;
      }
    }
    return hit_anything;
}

void HitableList::transform(const glm::vec3& translation, const glm::vec3& rotation)
{
	for (std::shared_ptr<Hitable>& hitable : list)
		hitable->transform(translation, rotation);
}

void HitableList::concatenateHitableList(std::shared_ptr<HitableList> hitableList)
{
	for (std::shared_ptr<Hitable>& hitable : hitableList->list)
		list.push_back(hitable);
}

bool HitableList::boundingBox(float t0, float t1, AABB& box) const
{
	AABB temp_box;
	if (list.size() == 0) {
		box = temp_box;
		return true;
	}

	if (list[0]->boundingBox(t0, t1, temp_box))
		box = temp_box;
	else
		return false;

	for (unsigned int i = 1; i < list.size(); ++i) {
		if (list[i]->boundingBox(t0, t1, temp_box))
			box = surrounding_box(box, temp_box);
		else
			return false;
	}
	return true;
}
