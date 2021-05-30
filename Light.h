#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

class Transform;
class HitRecord;
class Shape;
class Texture;

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
    virtual glm::vec3 sampleLi(glm::vec3* wi, const HitRecord& hit_record, float& pdf) const = 0;

    // Returns total emitted power of the light (or an estimation of it). Useful for importance sampling, for example.
    virtual glm::vec3 power() const = 0;

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
    virtual glm::vec3 sampleLi(glm::vec3* wi, const HitRecord& hit_record, float& pdf) const override;
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
    InfiniteAreaLight(const glm::vec3& emission, const std::string &texturePath);

public:
    // Inherited via Light
    virtual void transform(const Transform& transform) override;
    virtual glm::vec3 sampleLi(glm::vec3* wi, const HitRecord& hit_record, float& pdf) const override;
    virtual glm::vec3 power() const override;

private:
    const glm::vec3 _emission = glm::vec3(1);
    std::unique_ptr<Texture> _luminanceMap;
};
