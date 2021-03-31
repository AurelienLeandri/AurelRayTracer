#pragma once
#include "BxDF.h"
class OrenNayarReflection :
    public BxDF
{
public:
    OrenNayarReflection(const glm::vec3& albedo, float sigma);  // sigma standard deviation of the microfacets normal wrt the surface normal
    ~OrenNayarReflection() = default;

    // Inherited via BxDF
    virtual glm::vec3 f(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const;
    virtual glm::vec3 sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const;

private:
    glm::vec3 _albedo = glm::vec3(1, 1, 1);
    float _A = 0;
    float _B = 0;
};

