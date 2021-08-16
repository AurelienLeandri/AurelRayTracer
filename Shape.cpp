#include "Shape.h"

// Constructors/Destructors

Shape::Shape(Type type)
	: type(type)
{
}

// Accessors

int Shape::getMaterialId() const
{
	return _materialId;
}

void Shape::setMaterialId(int materialId)
{
	_materialId = materialId;
}
