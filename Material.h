#pragma once

#include <glm.hpp>

#include <memory>

#include "Texture.h"
#include "BSDF.h"

struct HitRecord;

class Material
{
public:
    Material();
    virtual ~Material();

public:
    virtual void getBSDF(HitRecord &hitRecord) const;
    virtual void emit(HitRecord &hitRecord) const;
    void recomputeBSDF();  // TODO: remove

    // Refactor when the archi gets better
    glm::vec3 color = glm::vec3(1, 1, 1);
    std::shared_ptr<Texture> albedo = nullptr;
    glm::vec3 albedoValue = glm::vec3(1, 1, 1);
    std::shared_ptr<Texture> specular = nullptr;
    std::shared_ptr<Texture> normal = nullptr;
    std::shared_ptr<Texture> height = nullptr;
    std::shared_ptr<Texture> emission = nullptr;
    glm::vec3 emissionValue = glm::vec3(0, 0, 0);

protected:
    BSDF _bsdf;
};

