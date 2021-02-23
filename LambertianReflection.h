#pragma once
#include "BxDF.h"

#include <memory>

class Texture;
struct HitRecord;

class LambertianReflection :
    public BxDF
{
public:
    LambertianReflection(glm::vec3 albedo, std::shared_ptr<Texture> albedoTexture);
    ~LambertianReflection();

    // Inherited via BxDF
    virtual glm::vec3 f(const glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord &hit_record) const;
    virtual glm::vec3 sample_f(glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord &hit_record) const;

private:
    glm::vec3 _albedoValue = glm::vec3(1, 1, 1);
    std::shared_ptr<Texture> _albedoTexture = nullptr;
};

