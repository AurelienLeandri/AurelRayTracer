#pragma once

#include "Hitable.h"
#include "HitableList.h"
#include "Vertex.h"

class Mesh :
    public Hitable
{
public:
    std::vector<Vertex> geometry;
    std::vector<unsigned int> indices;
    unsigned int materialId = 0;
};

