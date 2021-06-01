#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

class Transform;
class HitRecord;
class Shape;
class ImageTexture;
class Distribution2D;

enum class LightType {
    AREA,
    INFINITE_AREA
};

class Light
{
public:
    Light(LightType type);

public:
    // Transform the light using the given transformation matrix. The matrix is assumed not to be ill-formed. This function doesnt check for errors
    virtual void transform(const Transform& transform) = 0;

    // Returns the incident radiance arriving at the point given by the hit record, along a sampled vector wi (which is set by the function).
    virtual glm::vec3 sampleLi(glm::vec3& wi, const HitRecord& hit_record, float& pdf) const = 0;

    // Returns total emitted power of the light (or an estimation of it). Useful for importance sampling, for example.
    virtual glm::vec3 power() const = 0;

    // Returns radiance towards an arbitrary directions. Allows to get radiance for rays that did not hit anything, thus not relying on a HitRecord (for example for InfiniteAreaLights).
    virtual glm::vec3 radianceTowards(const glm::vec3 &w) const;

public:
    LightType getType() const;

protected:
    LightType _type;
};

class AreaLight : public Light
{
public:
    AreaLight(const glm::vec3& emission, std::shared_ptr<Shape> shape);

public:
    // Inherited via Light
    virtual void transform(const Transform& transform) override;
    virtual glm::vec3 sampleLi(glm::vec3& wi, const HitRecord& hit_record, float& pdf) const override;
    virtual glm::vec3 power() const override;

    // Returns the radiance along w coming a point on the light described by the given "pointOnLight" HitRecord.
    virtual glm::vec3 radianceAlongDirection(const HitRecord& pointOnLight, const glm::vec3& w) const;

private:
    const glm::vec3 _emission = glm::vec3(1);
    std::shared_ptr<Shape> _shape;
    const float _area = 0;
};

class InfiniteAreaLight : public Light
{
public:
    InfiniteAreaLight(const std::string &texturePath);

public:
    // Inherited via Light
    virtual void transform(const Transform& transform) override;
    virtual glm::vec3 sampleLi(glm::vec3& wi, const HitRecord& hit_record, float& pdf) const override;
    virtual glm::vec3 power() const override;
    virtual glm::vec3 radianceTowards(const glm::vec3& w) const override;


private:
    std::unique_ptr<ImageTexture> _luminanceMap;
    std::unique_ptr<ImageTexture> _radianceMap;
    std::unique_ptr<Distribution2D> _distribution;

    // TODO: compute the world's bounding sphere
    glm::vec3 worldCenter = glm::vec3(0);
    glm::vec3 worldradius = glm::vec3(2000);
};
