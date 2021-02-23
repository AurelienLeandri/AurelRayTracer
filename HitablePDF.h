#pragma once

#include "PDF.h"

#include "Hitable.h"

class HitablePDF : public PDF
{
public:
	HitablePDF(const glm::vec3& origin, const Hitable& hitable);

public:
	virtual float probabilityOf(const glm::vec3& outwardVector) const override;
	virtual glm::vec3 generate() const override;

private:
	glm::vec3 _origin;
	const Hitable& _hitable;
};

