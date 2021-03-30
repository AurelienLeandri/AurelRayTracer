#include "Material.h"

#include "HitRecord.h"
#include "LambertianReflection.h"
#include "FresnelSpecular.h"


Material::~Material() {}

MatteMaterial::MatteMaterial(std::shared_ptr<Texture> albedo) : _albedo(albedo) {
}

MatteMaterial::~MatteMaterial()
{
}

void MatteMaterial::getBSDF(HitRecord& hit_record) const
{
    hit_record.bsdf = BSDF();
    hit_record.bsdf.add(std::make_shared<LambertianReflection>(_albedo->getColor(hit_record)));
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
    hit_record.bsdf.add(std::make_shared<FresnelSpecular>(hit_record.ray.eta, _eta, _albedo->getColor(hit_record)));
}

