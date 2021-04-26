#pragma once
#include "BxDF.h"

#include <memory>

class Texture;
struct HitRecord;

class LambertianTransmission :
    public BxDF
{
public:
    LambertianTransmission(glm::vec3 albedo, float refraction_index, std::shared_ptr<Texture> albedoTexture);
    ~LambertianTransmission();

    // Inherited via BxDF
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const;
    virtual float pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const override;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const;

private:
    glm::vec3 _albedoValue = glm::vec3(1);
    float _refractionIndex;
    std::shared_ptr<Texture> _albedoTexture = nullptr;
};

