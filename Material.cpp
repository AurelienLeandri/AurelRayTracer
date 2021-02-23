#include "Material.h"

#include "HitRecord.h"
#include "LambertianReflection.h"


Material::Material()
{
    _bsdf.add(new LambertianReflection(albedoValue, albedo));
}


Material::~Material()
{
}

void Material::getBSDF(HitRecord& hitRecord) const
{
    hitRecord.bsdf = &_bsdf;
}

void Material::emit(HitRecord& hitRecord) const
{
    if (emission) {
        hitRecord.emission = emission->color(hitRecord.u, hitRecord.v, hitRecord.position);
    }
    else {
        hitRecord.emission = emissionValue;
    }
}

void Material::recomputeBSDF()
{
    _bsdf = BSDF();
    _bsdf.add(new LambertianReflection(albedoValue, albedo));
}
