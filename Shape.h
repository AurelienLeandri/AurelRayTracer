#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Embree.h"

struct HitRecord;
class Transform;

// Abstract
class Shape
{
// Class-specific declarations
public:
	enum class Type {
		TRIANGLE,
		MESH,
		SPHERE
	};

// Attributes
protected:
	int _materialId = 0;
public:
	const Type type;

// Constructors/Destructors
public:
	Shape(Type type);

// Interface
public:
	// Give the area of the shape
	virtual float area() const = 0;

	// Sample a point on the shape, gives the probability density of the sample created this way (by calling pdf(), see below)
	virtual glm::vec3 sample(const HitRecord &record, float &pdf) const = 0;

	// Give the probability density of a given sample point (value) on the shape
	virtual float pdf(const glm::vec3& point, const HitRecord& record) const = 0;

	// Transform the shape using the given transformation matrix. The matrix is assumed to be well formed. This function doesn't check for errors
	virtual void transform(const Transform &transform) = 0;

// Accessors
public:
	int getMaterialId() const;
	void setMaterialId(int materialId);
};

