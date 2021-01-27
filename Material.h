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
    //virtual void getBSDF(hit_record &hitRecord) const;
    //virtual void emit(hit_record &hitRecord) const;

    glm::vec3 color = glm::vec3(1, 1, 1);  // For testing at the beginning. Remove whenthe archigets better
    std::shared_ptr<Texture> texture = nullptr;  // For testing at the beginning. Remove whenthe archigets better

protected:
    //BSDF _bsdf;
};

