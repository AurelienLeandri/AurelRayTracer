#include "Light.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "HitRecord.h"
#include "Shape.h"
#include "Texture.h"

AreaLight::AreaLight(const glm::vec3& emission, std::shared_ptr<Shape> shape)
    : Light(LightType::AREA), _emission(emission), _shape(shape), _area(shape->area())
{
}

void AreaLight::transform(const Transform& transform)
{
}

glm::vec3 AreaLight::sampleLi(glm::vec3* wi, const HitRecord& hit_record, float& pdf) const
{
    return glm::vec3();
}

glm::vec3 AreaLight::power() const
{
    return _emission * _area * (float)M_PI;
}

glm::vec3 AreaLight::radianceAlongDirection(const HitRecord& pointOnLight, const glm::vec3& w) const
{
    if (glm::dot(w, pointOnLight.normal) < 0)
        return glm::vec3(0);
    return _emission;
}

InfiniteAreaLight::InfiniteAreaLight(const glm::vec3& emission, const std::string& texturePath)
    : Light(LightType::INFINITE_AREA), _emission(emission)
{
	// Initialization of the light map
    _luminanceMap = std::make_unique<ImageTexture>(texturePath, ImageTextureDataType::FLOAT, ImageTextureLayoutType::LUMINANCE);
}

void InfiniteAreaLight::transform(const Transform& transform)
{
}

glm::vec3 InfiniteAreaLight::sampleLi(glm::vec3* wi, const HitRecord& hit_record, float& pdf) const
{
    return glm::vec3();
}

glm::vec3 InfiniteAreaLight::power() const
{
    return glm::vec3();
}

Light::Light(LightType type)
    : _type(type)
{
}

LightType Light::getType() const
{
    return _type;
}
