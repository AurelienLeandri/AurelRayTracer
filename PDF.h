#pragma once

#include <glm.hpp>

class PDF
{
public:
	virtual float probabilityOf(const glm::vec3& outwardVector) const = 0;
	virtual glm::vec3 generate() const = 0;
};

