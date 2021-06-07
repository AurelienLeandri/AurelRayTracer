#include "Light.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "HitRecord.h"
#include "Shape.h"
#include "Utils.h"
#include "Texture.h"
#include "Distribution2D.h"

AreaLight::AreaLight(const glm::vec3& emission, std::shared_ptr<Shape> shape)
    : Light(LightType::AREA), _emission(emission), _shape(shape), _area(shape->area())
{
}

void AreaLight::transform(const Transform& transform)
{
}

glm::vec3 AreaLight::sampleLi(glm::vec3& wi, const HitRecord& hit_record, float& pdf) const
{
    wi = _shape->sample(hit_record, pdf);
    return _emission;
}

glm::vec3 AreaLight::power() const
{
    return _emission * _area * (float)M_PI;
}

glm::vec3 AreaLight::radianceFromPoint(const HitRecord& pointOnLight, const glm::vec3& w) const
{
    if (glm::dot(w, pointOnLight.normal) < 0)
        return glm::vec3(0);
    return _emission;
}

InfiniteAreaLight::InfiniteAreaLight(const std::string& texturePath)
    : Light(LightType::INFINITE_AREA)
{
	// Initialization of the light map
    _luminanceMap = std::make_unique<ImageTexture>(texturePath, ImageTextureDataType::FLOAT, ImageTextureLayoutType::LUMINANCE);
    _radianceMap = std::make_unique<ImageTexture>(texturePath, ImageTextureDataType::FLOAT, ImageTextureLayoutType::RGB);

    // Adjust the pdf values for taking into acount that the luminance map is projected onto a sphere.
    int width = _luminanceMap->getWidth();
    int height = _luminanceMap->getHeight();
    float* data = (float*)_luminanceMap->getData();
    for (int i = 0; i < height; ++i) {
        float vp = (float)i / (float)height;
        float sinTheta = std::sin((float)M_PI * float(i + .5f) / float(height));
        for (int j = 0; j < width; ++j) {
            data[i * width + j] *= sinTheta;
        }
    }

    _distribution = std::make_unique<Distribution2D>(data, height, width);
}

void InfiniteAreaLight::transform(const Transform& transform)
{
}

glm::vec3 InfiniteAreaLight::sampleLi(glm::vec3& wi, const HitRecord& hit_record, float& pdf) const
{
    pdf = 0;
    int iu, iv;
    pdf = _distribution->sample(frand(), frand(), iu, iv);
    if (pdf == 0)
        return glm::vec3(0);

    float u = (float)iu / _radianceMap->getWidth();
    float v = (float)iv / _radianceMap->getHeight();

    float theta = v * (float)M_PI, phi = u * 2 * (float)M_PI;
    float cosTheta = std::cos(theta), sinTheta = std::sin(theta);
    float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
    wi = glm::vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);

    if (sinTheta == 0)
        return glm::vec3(0);

    pdf = pdf / (2 * (float)M_PI * (float)M_PI * sinTheta);

    return _radianceMap->getColor(u, v);
}

glm::vec3 InfiniteAreaLight::power() const
{
    return glm::vec3();
}

glm::vec3 InfiniteAreaLight::radianceInDirection(const glm::vec3& w) const
{
    glm::vec3 local_w = glm::normalize(w);
    static float INV_M_PI = 1 / (float)M_PI;
    float u = glm::pow(glm::atan(local_w.y, local_w.x), 2);
    if (u < 0) u += 2 * (float)M_PI;
    u *= INV_M_PI;
    float v = glm::acos(glm::clamp(local_w.z, -1.f, 1.f));
    v *= INV_M_PI;
    return _radianceMap->getColor(u, v);
}

Light::Light(LightType type)
    : _type(type)
{
}

glm::vec3 Light::radianceInDirection(const glm::vec3& w) const
{
    return glm::vec3(0);
}

LightType Light::getType() const
{
    return _type;
}
