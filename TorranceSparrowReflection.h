#pragma once
#include "BxDF.h"

#include <memory>

class MicrofacetDistribution;

class TorranceSparrowReflection :
    public BxDF
{
public:
    TorranceSparrowReflection(float etaRay, float etaInterface, const glm::vec3 &albedo, std::shared_ptr<MicrofacetDistribution> reflection_model, float k = 0);

public:
    // Inherited via BxDF
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const override;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const override;

private:
    glm::vec3 _albedo;
    std::shared_ptr<MicrofacetDistribution> _reflectionModel;
    float _etaRay = 0;
    float _etaInterface = 0;
    float _k = 0;  // Imaginary part for conductors
};

