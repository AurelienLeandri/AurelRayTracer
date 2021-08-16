#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Embree.h"

struct HitRecord;
class Transform;

/**
 * Abstract class representing an arbitrary shape.
 */
/* abstract */ class Shape
{
// Class-specific declarations
public:
	/**
	* All shape types available. It is set automatically and is used to identify a shape.
	*/
	enum class Type {
		TRIANGLE,
		MESH,
		SPHERE
	};

// Attributes
protected:
	/**
	 * The id of the material associated to the Shape in the scene, if any.
	 */
	int _materialId = 0;
public:
	/**
	 * The type of the Shape.
	 */
	const Type type;

// Constructors/Destructors
protected:
	/**
	* Creates a shape of the given type
	 * \param type The type of the shape
	 */
	Shape(Type type);

// Interface
public:
	/**
	 * Samples a point on the shape, gives the probability density of the sample created this way.
	 * \param record Data about the intersection point from which the point is sampled.
	 * \param pdf The probability density of picking the point returned by the function.
	 * \return A point on the shape
	 */
	virtual glm::vec3 sample(const HitRecord &record, float &pdf) const = 0;

	/**
	 * Gives the probability density of picking a given sample point on the shape.
	 * \param point The point of which the pdf has to be evaluated
	 * \param record Data about the intersection point from which point is sampled.
	 * \return The value of the pdf for the given point
	 */
	virtual float pdf(const glm::vec3& point, const HitRecord& record) const = 0;

	/**
	 * Gives the area of the shape.
	 * \return The area of the shape
	 */
	virtual float area() const = 0;

	/**
	 * Transforms the shape using the given transformation matrix. This function doesn't check for errors related to ill-formed transformation matrices.
	 * \param transform A transformation matrix. It is assumed to be well formed for a linear transformation.
	 */
	virtual void transform(const Transform &transform) = 0;

// Accessors
public:
	int getMaterialId() const;
	void setMaterialId(int materialId);
};

