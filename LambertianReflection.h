#pragma once
#include "BxDF.h"

#include <memory>

class Texture;
struct HitRecord;

class LambertianReflection :
    public BxDF
{
public:
    LambertianReflection(const glm::vec3 &albedo);
    ~LambertianReflection();

    // Inherited via BxDF
    virtual glm::vec3 f(const glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord &hit_record) const;

private:
    glm::vec3 albedo = glm::vec3(1);
};

