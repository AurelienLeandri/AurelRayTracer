#include "BxDF.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/glm.hpp>

#include "Utils.h"

BxDF::BxDF(Type type) : type(type)
{
}


BxDF::~BxDF()
{
}

float BxDF::pdf(const glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record) const
{
    if (w_i.z * w_o.z < 0)
        return 0;
    return glm::abs(w_i.z) / float(M_PI);
}

glm::vec3 BxDF::sample_f(glm::vec3& w_i, const glm::vec3& w_o, const HitRecord& hit_record, float& pdf) const
{
    /*
    do {
        w_i = glm::normalize(glm::vec3(0, 0, 1) + glm::normalize(random_in_unit_sphere()));
    } while (w_i.z - 0.0001f < 0.f);
    if (w_o.z <= 0.f)
        w_i.z *= -1;
    pdf = this->pdf(w_i, w_o, hit_record);
    return f(w_i, w_o, hit_record);
    */

    // Cosine-sample the hemisphere, flipping the direction if necessary
    //w_i = random_cosine_direction();
    w_i = glm::normalize(PBRTCosineSampleHemisphere(frand(), frand()));
    //w_i = glm::normalize(random_in_unit_sphere());
    //if (w_o.z < 0.) w_i.z *= -1.f;
    if (w_o.z * w_i.z < 0.) w_i.z *= -1.f;
    pdf = this->pdf(w_i, w_o, hit_record);
    //pdf = 0.5f / float(M_PI);
    return f(w_i, w_o, hit_record);
}

BxDF::Type::Type(int flags)
    : flags(flags)
{
}

bool BxDF::Type::isType(Type type2) const
{
    return (flags | type2.flags);
}

bool operator==(const BxDF::Type& lhs, const BxDF::Type& rhs)
{
    return lhs.flags == rhs.flags;
}

bool operator!=(const BxDF::Type& lhs, const BxDF::Type& rhs)
{
    return !(rhs == lhs);
}
