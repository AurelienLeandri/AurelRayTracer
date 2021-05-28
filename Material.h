#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Texture.h"
#include "BSDF.h"

struct HitRecord;

class Material
{
public:
    virtual ~Material();

public:
    virtual void getBSDF(HitRecord &hitRecord) const = 0;
    virtual void emit(HitRecord &hitRecord) const;
};

class MatteMaterial : public Material
{
public:
    MatteMaterial(std::shared_ptr<Texture> albedo);
    virtual ~MatteMaterial();

public:
    virtual void getBSDF(HitRecord& hitRecord) const;

protected:
    std::shared_ptr<Texture> _albedo = ConstantTexture::white;
};

class EmissiveMaterial : public Material
{
public:
    EmissiveMaterial(std::shared_ptr<Texture> emission, std::shared_ptr<Texture> albedo = ConstantTexture::white);
    virtual ~EmissiveMaterial();

public:
    virtual void getBSDF(HitRecord& hitRecord) const;
    virtual void emit(HitRecord& hitRecord) const override;

protected:
    std::shared_ptr<Texture> _emission = ConstantTexture::black;
    std::shared_ptr<Texture> _albedo = ConstantTexture::white;
};

class Dielectric : public Material {
public:
    Dielectric(float eta, std::shared_ptr<Texture> albedo = ConstantTexture::white);

public:
    virtual void getBSDF(HitRecord& hitRecord) const;

private:
    float _eta = 1;
    std::shared_ptr<Texture> _albedo = ConstantTexture::white;
};

