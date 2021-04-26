#include "Material.h"

#include "HitRecord.h"
#include "LambertianReflection.h"
#include "OrenNayarReflection.h"
#include "Specular.h"
#include "TrowbridgeReitz.h"
#include "BeckmannSpizzichino.h"
#include "TorranceSparrowReflection.h"
#include "TorranceSparrowTransmission.h"
#include "FresnelBlend.h"


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
    //hit_record.bsdf.add(std::make_shared<LambertianReflection>(_albedo->getColor(hit_record)));
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
    //hit_record.bsdf.add(std::make_shared<FresnelBlend>(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.3f, 0.5f), std::make_shared<TrowbridgeReitz>(MicrofacetDistribution::roughnessToAlpha(0.1f))));
    hit_record.bsdf.add(std::make_shared<TorranceSparrowReflection>(hit_record.ray.eta, 300.f, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(MicrofacetDistribution::roughnessToAlpha(0.5f)), 1.8f));
    //hit_record.bsdf.add(std::make_shared<TorranceSparrowReflection>(hit_record.ray.eta, 2.56f, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(MicrofacetDistribution::roughnessToAlpha(0.1f))));
 
    //hit_record.bsdf.add(std::make_shared<TorranceSparrowTransmission>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(MicrofacetDistribution::roughnessToAlpha(0.2f))));
    //hit_record.bsdf.add(std::make_shared<SpecularTransmission>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record)));
    //hit_record.bsdf.add(std::make_shared<SpecularReflection>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record)));
}

