#pragma once
#include "BxDF.h"

#include <memory>

class MicrofacetDistribution;

class TorranceSparrowReflection :
    public BxDF
{
public:
    TorranceSparrowReflection(float etaRay, float etaInterface, const glm::vec3& albedo, std::shared_ptr<MicrofacetDistribution> reflection_model, float k = 0);
    TorranceSparrowReflection(const glm::vec3& etaRay, const glm::vec3& etaInterface, const glm::vec3& albedo, std::shared_ptr<MicrofacetDistribution> reflection_model, const glm::vec3& k = glm::vec3(0));

public:
    // Inherited via BxDF
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const override;
    virtual float pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const override;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const override;

private:
    glm::vec3 _albedo;
    std::shared_ptr<MicrofacetDistribution> _reflectionModel;
    glm::vec3 _etaRay = glm::vec3(0);
    glm::vec3 _etaInterface = glm::vec3(0);
    glm::vec3  _k = glm::vec3(0);  // Imaginary part for conductors
};

