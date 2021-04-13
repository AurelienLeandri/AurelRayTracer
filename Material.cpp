#include "Material.h"

#include "HitRecord.h"
#include "LambertianReflection.h"
#include "OrenNayarReflection.h"
#include "FresnelSpecular.h"
#include "TrowbridgeReitz.h"
#include "BeckmannSpizzichino.h"
#include "TorranceSparrowReflection.h"
#include "TorranceSparrowTransmission.h"


Material::~Material() {}

MatteMaterial::MatteMaterial(std::shared_ptr<Texture> albedo) : _albedo(albedo) {
}

MatteMaterial::~MatteMaterial()
{
}

void MatteMaterial::getBSDF(HitRecord& hit_record) const
{
    hit_record.bsdf = BSDF();
    hit_record.bsdf.add(std::make_shared<OrenNayarReflection>(_albedo->getColor(hit_record), (float)(3.1415) / 2));
}

EmissiveMaterial::EmissiveMaterial(std::shared_ptr<Texture> emission, std::shared_ptr<Texture> albedo) : _emission(emission), _albedo(albedo) {
}

EmissiveMaterial::~EmissiveMaterial()
{
}

void EmissiveMaterial::getBSDF(HitRecord& hit_record) const
{
    hit_record.bsdf = BSDF();
    hit_record.bsdf.add(std::make_shared<LambertianReflection>(_albedo->getColor(hit_record)));
}

void EmissiveMaterial::emit(HitRecord& hit_record) const
{
    hit_record.emission = _emission->getColor(hit_record);
}

void Material::emit(HitRecord& hitRecord) const
{
}

Dielectric::Dielectric(float eta, std::shared_ptr<Texture> albedo) : _eta(eta), _albedo(albedo)
{
}

void Dielectric::getBSDF(HitRecord& hit_record) const
{
    hit_record.bsdf = BSDF();
    //hit_record.bsdf.add(std::make_shared<TorranceSparrowReflection>(hit_record.ray.eta, 0, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(MicrofacetDistribution::roughnessToAlpha(0.1f))));
 
    hit_record.bsdf.add(std::make_shared<TorranceSparrowTransmission>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(0)));
    //hit_record.bsdf.add(std::make_shared<FresnelSpecular>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record)));
}

