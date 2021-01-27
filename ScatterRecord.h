#pragma once

#include "glm.hpp"

#include "ray.h"
//#include "PDF.h"

#include <memory>

struct ScatterRecord {
	Ray specular_ray;
	bool is_specular = false;
	glm::vec3 attenuation;
	float sampleWeight = 0.f;
  glm::vec3 scatterDirection;
};
