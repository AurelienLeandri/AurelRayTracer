#pragma once
#include "BxDF.h"

#include <glm.hpp>

float fresnelDielectric(float cos_w_i_normal, float eta_i, float eta_t);
glm::vec3 fresnelConductor(float cos_w_normal, const glm::vec3& eta_i, const glm::vec3& eta_t, const glm::vec3& k);

class SpecularTransmission :
    public BxDF
{
public:
    SpecularTransmission(float etaRay, float etaInterface, const glm::vec3& albedo, float k = 0, bool fromLight = false);

public:
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float &pdf) const;

private:
    float _etaRay = 0;
    float _etaInterface = 0;
    glm::vec3 _albedo = glm::vec3(1);
    bool _fromLight = false;
};

class SpecularReflection :
    public BxDF
{
public:
    SpecularReflection(float etaRay, float etaInterface, const glm::vec3& albedo, float k = 0, bool fromLight = false);
    SpecularReflection(const glm::vec3& etaRay, const glm::vec3& etaInterface, const glm::vec3& albedo, const glm::vec3& k = glm::vec3(0), bool fromLight = false);

public:
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const;

private:
    glm::vec3 _etaRay = glm::vec3(0);
    glm::vec3 _etaInterface = glm::vec3(0);
    glm::vec3 _albedo = glm::vec3(1);
    glm::vec3 _k = glm::vec3(0);
    bool _fromLight = false;
};

