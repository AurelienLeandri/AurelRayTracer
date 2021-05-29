#pragma once

#include <glm/glm.hpp>
#include <memory>

class HitRecord;
class Transform;

class Shape
{
public:
	// Gives area of the shape
	virtual float area() const = 0;

	// Sample a point on the shape, gives the probability density of the sample created this way (by calling pdf(), see below)
	virtual glm::vec3 sample(const HitRecord &record, float &pdf) const = 0;

	// Gives the probability density of a given sample point (value) on the shape
	virtual float pdf(const glm::vec3& point, const HitRecord& record) const = 0;

	// Transform the shape using the given transformation matrix. The matrix is assumed not to be ill-formed. This function doesnt check for errors
	virtual void transform(const Transform &transform) = 0;
};

