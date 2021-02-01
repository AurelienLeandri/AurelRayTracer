#pragma once

#include <glm.hpp>

#include <memory>

#include "Texture.h"

struct HitRecord;

class Material
{
public:
    Material();
    virtual ~Material();

public:
    //virtual void getBSDF(HitRecord &hitRecord) const;
    //virtual void emit(HitRecord &hitRecord) const;

    // Refactor when the archi gets better
    glm::vec3 color = glm::vec3(1, 1, 1);
    std::shared_ptr<Texture> diffuse = nullptr;
    std::shared_ptr<Texture> specular = nullptr;
    std::shared_ptr<Texture> normal = nullptr;
    std::shared_ptr<Texture> height = nullptr;

protected:
    //BSDF _bsdf;
};

