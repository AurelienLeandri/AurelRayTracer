#pragma once
#include "BxDF.h"

#include <memory>

class MicrofacetDistribution;

class FresnelBlend :
    public BxDF
{
public:
    FresnelBlend(const glm::vec3& fresnel_reflection, const glm::vec3& diffuse_reflection, std::shared_ptr<MicrofacetDistribution> _reflection_model);

public:
    // Inherited via BxDF
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const override;
    virtual float pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const override;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const override;

private:
    glm::vec3 _fresnelReflection = glm::vec3(0);
    glm::vec3 _diffuseReflection = glm::vec3(0);
    std::shared_ptr<MicrofacetDistribution> _reflectionModel = nullptr;
};

