#pragma once

#include <glm/glm.hpp>

float frand();

glm::vec3 random_in_unit_sphere();

glm::vec3 random_in_unit_disc();

glm::vec3 reflect(const glm::vec3& v, const glm::vec3& normal);

bool refract(const glm::vec3& v, const glm::vec3 &normal, float ni_over_nt, glm::vec3 &refracted);

float trilinear_interpolation(float c[2][2][2], float x_d, float y_d, float z_d);

float linear_interpolation(float a, float b, float ratio);

void get_sphere_uv(const glm::vec3& n, float& u, float& v);

glm::vec3 spherical_to_cartesian(float theta, float phi);

glm::vec3 random_cosine_direction();