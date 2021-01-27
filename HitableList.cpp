
#include "HitableList.h"

//#include "AABB.h"

bool HitableList::hit(Ray r, float t_min, float t_max, HitRecord & record) const
{
    float min_t = t_max;
    bool hit_anything = false;
    HitRecord candidate_hit_record;
    for (const auto &hitable : list) {
      if (hitable->hit(r, t_min, min_t, candidate_hit_record)) {
        hit_anything = true;
        record = candidate_hit_record;
        min_t = candidate_hit_record.t;
      }
    }
    return hit_anything;
}

/*
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
*/
