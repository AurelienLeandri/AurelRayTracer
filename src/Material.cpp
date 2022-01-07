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

void Material::emit(HitRecord& hitRecord) const
{
    hitRecord.emission = glm::vec3(0);
}

PerfectDiffuseMaterial::PerfectDiffuseMaterial(std::shared_ptr<Texture> albedo)
    : _albedo(albedo)
{
}

void PerfectDiffuseMaterial::getBSDF(HitRecord& hitRecord) const
{
    hitRecord.bsdf = BSDF();
    hitRecord.bsdf.add(std::make_shared<LambertianReflection>(_albedo->getColor(hitRecord)));
}

PerfectSpecularMaterial::PerfectSpecularMaterial(std::shared_ptr<Texture> albedo)
    : _albedo(albedo)
{
}

void PerfectSpecularMaterial::getBSDF(HitRecord& hitRecord) const
{
    hitRecord.bsdf = BSDF();
    hitRecord.bsdf.add(std::make_shared<TorranceSparrowReflection>(hitRecord.ray.eta, 0.f, _albedo->getColor(hitRecord), std::make_shared<TrowbridgeReitz>(0.f), 0.f));
}

PerfectTransparentMaterial::PerfectTransparentMaterial(std::shared_ptr<Texture> albedo)
    : _albedo(albedo)
{
}

void PerfectTransparentMaterial::getBSDF(HitRecord& hitRecord) const
{
    hitRecord.bsdf = BSDF();
    // TODO: code labertian transmission BTDF
    //hitRecord.bsdf.add(std::make_shared<Specular>(hitRecord.ray.eta, _eta, _albedo);
}

MatteMaterial::MatteMaterial(std::shared_ptr<Texture> albedo, float sigma)
    : _albedo(albedo), _sigma(sigma)
{
}

void MatteMaterial::getBSDF(HitRecord& hit_record) const
{
    hit_record.bsdf = BSDF();
    hit_record.bsdf.add(std::make_shared<OrenNayarReflection>(_albedo->getColor(hit_record), _sigma));
}

EmissiveMaterial::EmissiveMaterial(std::shared_ptr<Texture> emission, std::shared_ptr<Texture> albedo) : _emission(emission), _albedo(albedo) {
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

DielectricMaterial::DielectricMaterial(float eta, std::shared_ptr<Texture> albedo, float roughness, float k)
    : _eta(eta), _roughness(roughness), _k(k), _albedo(albedo), _alpha(MicrofacetDistribution::roughnessToAlpha(_roughness))
{
}

void DielectricMaterial::getBSDF(HitRecord& hit_record) const
{
    hit_record.bsdf = BSDF();
    hit_record.bsdf.add(std::make_shared<TorranceSparrowReflection>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(_alpha), _k));
    hit_record.bsdf.add(std::make_shared<TorranceSparrowTransmission>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(_alpha)));
}

GlossyMaterial::GlossyMaterial(float eta, std::shared_ptr<Texture> albedo, float roughness)
    : _eta(eta), _roughness(roughness), _albedo(albedo), _alpha(MicrofacetDistribution::roughnessToAlpha(_roughness))
{
}

void GlossyMaterial::getBSDF(HitRecord& hit_record) const
{
    hit_record.bsdf = BSDF();
    hit_record.bsdf.add(std::make_shared<LambertianReflection>(_albedo->getColor(hit_record)));
    hit_record.bsdf.add(std::make_shared<TorranceSparrowReflection>(hit_record.ray.eta, _eta, glm::vec3(1.f), std::make_shared<TrowbridgeReitz>(_alpha)));
}

/*
void DielectricMaterial::getBSDF(HitRecord& hit_record) const
{
    hit_record.bsdf = BSDF();
    //hit_record.bsdf.add(std::make_shared<FresnelBlend>(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.3f, 0.5f), std::make_shared<TrowbridgeReitz>(MicrofacetDistribution::roughnessToAlpha(0.1f))));
    //hit_record.bsdf.add(std::make_shared<TorranceSparrowReflection>(hit_record.ray.eta, 300.f, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(MicrofacetDistribution::roughnessToAlpha(0.5f)), 1.8f));
    //hit_record.bsdf.add(std::make_shared<TorranceSparrowReflection>(hit_record.ray.eta, 2.56f, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(MicrofacetDistribution::roughnessToAlpha(0.1f))));
 
    //hit_record.bsdf.add(std::make_shared<TorranceSparrowTransmission>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record), std::make_shared<TrowbridgeReitz>(MicrofacetDistribution::roughnessToAlpha(0.2f))));
    //hit_record.bsdf.add(std::make_shared<SpecularTransmission>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record)));
    hit_record.bsdf.add(std::make_shared<SpecularReflection>(hit_record.ray.eta, 300, _albedo->getColor(hit_record), 1.8));

    // Plastic params
    //hit_record.bsdf.add(std::make_shared<LambertianReflection>(glm::vec3(0.9f, 0.4f, 0.8f)));
    //hit_record.bsdf.add(std::make_shared<TorranceSparrowReflection>(hit_record.ray.eta, 1.5f, glm::vec3(1.f), std::make_shared<TrowbridgeReitz>(0.1f)));
}
*/


