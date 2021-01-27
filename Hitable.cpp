#include "Hitable.h"

#include <limits>

Hitable::Hitable(std::shared_ptr<Material> material) : material(material)
{
}

bool Hitable::hit(Ray r, HitRecord& record) const
{
	return hit(r, 0, std::numeric_limits<float>::max(), record);
}
