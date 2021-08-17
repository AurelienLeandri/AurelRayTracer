#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Texture.h"
#include "BSDF.h"

struct HitRecord;

class Material
{
public:
    virtual void getBSDF(HitRecord &hitRecord) const = 0;
    virtual void emit(HitRecord &hitRecord) const;
};

class PerfectDiffuseMaterial : public Material {
public:
    PerfectDiffuseMaterial(std::shared_ptr<Texture> albedo = ConstantTexture::white);

public:
    virtual void getBSDF(HitRecord& hitRecord) const;

public:
    std::shared_ptr<Texture> albedo;
};

class PerfectSpecularMaterial : public Material {
public:
    PerfectSpecularMaterial(std::shared_ptr<Texture> albedo = ConstantTexture::white);

public:
    virtual void getBSDF(HitRecord& hitRecord) const;

public:
    std::shared_ptr<Texture> albedo;
};

class PerfectTransparentMaterial : public Material {
public:
    PerfectTransparentMaterial(std::shared_ptr<Texture> albedo = ConstantTexture::white);

public:
    virtual void getBSDF(HitRecord& hitRecord) const;

public:
    std::shared_ptr<Texture> albedo;
};

class MatteMaterial : public Material
{
public:
    MatteMaterial(std::shared_ptr<Texture> albedo = ConstantTexture::white, float sigma = 3.1415f / 2);

public:
    virtual void getBSDF(HitRecord& hitRecord) const;

public:
    std::shared_ptr<Texture> albedo;
    float _sigma;
};

class EmissiveMaterial : public Material
{
public:
    EmissiveMaterial(std::shared_ptr<Texture> emission, std::shared_ptr<Texture> albedo = ConstantTexture::black);

public:
    virtual void getBSDF(HitRecord& hitRecord) const;
    virtual void emit(HitRecord& hitRecord) const override;

public:
    std::shared_ptr<Texture> emission;
    std::shared_ptr<Texture> albedo;
};

class DielectricMaterial : public Material {
public:
    DielectricMaterial(float eta, std::shared_ptr<Texture> albedo = ConstantTexture::white, float roughness = 0, float k = 0);

public:
    virtual void getBSDF(HitRecord& hitRecord) const;

public:
    float eta;
    float roughness;
    float alpha;
    float k;
    std::shared_ptr<Texture> albedo;
};

class GlossyMaterial : public Material {
public:
    GlossyMaterial(float eta, std::shared_ptr<Texture> albedo = ConstantTexture::white, float roughness = 0);

public:
    virtual void getBSDF(HitRecord& hitRecord) const;

public:
    float eta;
    float roughness;
    float alpha;
    std::shared_ptr<Texture> albedo;
};
