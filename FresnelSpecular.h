#pragma once
#include "BxDF.h"

#include <glm.hpp>

class FresnelSpecular :
    public BxDF
{
public:
    FresnelSpecular(float etaRay, float etaInterface, const glm::vec3& albedo, bool fromLight = false, float k = 0);

public:
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float &pdf) const;

public:
    static float fresnelDielectric(float cos_w_i_normal, float eta_i, float eta_t);
    static glm::vec3 fresnelConductor(float cos_w_normal, const glm::vec3 &eta_i, const glm::vec3& eta_t, const glm::vec3& k);

private:
    glm::vec3 _etaRay = glm::vec3(0);
    glm::vec3 _etaInterface = glm::vec3(0);
    glm::vec3 _k = glm::vec3(0);  // Imaginary part for conductors
    glm::vec3 _albedo = glm::vec3(1, 1, 1);
    bool _fromLight = false;
};

