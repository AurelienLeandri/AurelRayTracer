#include "LambertianReflection.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Utils.h"
#include "HitRecord.h"
#include "Texture.h"

LambertianReflection::LambertianReflection(const glm::vec3 &albedo) :
    BxDF(BxDF::Type::BSDF_REFLECTION | BxDF::Type::BSDF_DIFFUSE),
    _albedo(albedo)
{
}


LambertianReflection::~LambertianReflection()
{
}

glm::vec3 LambertianReflection::f(const glm::vec3 & w_i, const glm::vec3 & w_o, const HitRecord& hit_record) const
{
    // When one integrates the hemispherical-directional function over the hemisphere, we get a pi factor, hence the 1 / pi.
    return _albedo / float(M_PI);
}

