#pragma once
#include "BxDF.h"
class FresnelSpecular :
    public BxDF
{
public:
    FresnelSpecular(float etaRay, float etaInterface, const glm::vec3& albedo, bool fromLight = false);

public:
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float &pdf) const;

private:
    float _etaRay = 0;
    float _etaInterface = 0;
    float _k = 0;  // Imaginary part for conductors
    glm::vec3 _albedo = glm::vec3(1, 1, 1);
    bool _fromLight = false;
};

