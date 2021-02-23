#include "HitablePDF.h"

HitablePDF::HitablePDF(const glm::vec3& origin, const Hitable& hitable) : _origin(origin), _hitable(hitable)
{
}

float HitablePDF::probabilityOf(const glm::vec3& outwardVector) const
{
	return 1.f;  // TODO
	//return _hitable.pdf_value(_origin, outwardVector);
}

glm::vec3 HitablePDF::generate() const
{
	return _hitable.random(_origin);
}
