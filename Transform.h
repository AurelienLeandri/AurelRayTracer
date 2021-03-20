#pragma once

#include <glm.hpp>

struct Transform {
    glm::vec3 translation = glm::vec3(0, 0, 0);
    glm::vec3 rotation_rads = glm::vec3(0, 0, 0);
    glm::vec3 scaling = glm::vec3(1, 1, 1);
};