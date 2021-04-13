
#include "Utils.h"

#include <glm.hpp>

#include <random>

#define _USE_MATH_DEFINES
#include <math.h>

float frand()
{
	return static_cast<float>(rand()) / (RAND_MAX + 1);
}

glm::vec3 random_in_unit_sphere()
{
	glm::vec3 result;
	do {
		result = 2.f * (glm::vec3(frand(), frand(), frand())) - glm::vec3(1);
	} while (glm::length(result) >= 1.f);
	return result;
}

glm::vec3 random_on_unit_sphere()
{
    glm::vec3 result;
    do {
        result = 2.f * (glm::vec3(frand(), frand(), frand())) - glm::vec3(1);
    } while (glm::length(result) >= 1.f);
    return glm::normalize(result);
}

glm::vec3 random_in_unit_disc()
{
	glm::vec3 result;
	do {
		result = 2.f * glm::vec3(frand(), frand(), 0) - glm::vec3(1.f, 1.f, 0.f);
	} while (dot(result, result) >= 1.f);
	return result;
}

glm::vec3 reflect(const glm::vec3& v, const glm::vec3& normal)
{
	return -v + 2.f * dot(v, normal) * normal;
}

bool refract(const glm::vec3& v, const glm::vec3& normal, float ni_over_nt, glm::vec3& refracted)
{
	float cos_theta_i = glm::dot(normal, v);
	float sin2_theta_i = glm::max(0.f, 1.f - cos_theta_i * cos_theta_i);
	float sin2_theta_t = ni_over_nt * ni_over_nt * sin2_theta_i;
	if (sin2_theta_t >= 1)
		return false;  // total internal reflection
	float cos_theta_t = glm::sqrt(1.f - sin2_theta_t);
	refracted = ni_over_nt * -v + (ni_over_nt * cos_theta_i - cos_theta_t) * normal;
	return true;
}

float trilinear_interpolation(float c[2][2][2], float x_d, float y_d, float z_d)
{
	float c2[2][2] = {
		{linear_interpolation(c[0][0][0], c[1][0][0], x_d), linear_interpolation(c[0][1][0], c[1][1][0], x_d)},
		{linear_interpolation(c[0][0][1], c[1][0][1], x_d), linear_interpolation(c[0][1][1], c[1][1][1], x_d)}
	};
	float c1[2] = {
		linear_interpolation(c2[0][0], c2[0][1], y_d),
		linear_interpolation(c2[1][0], c2[1][1], y_d)
	};
	return linear_interpolation(c1[0], c1[1], z_d);
}

float linear_interpolation(float a, float b, float ratio)
{
	return a + (b - a) * ratio;
}

void get_sphere_uv(const glm::vec3& n, float & u, float & v)
{
	u = atan2(n.x, n.z) / (2 * static_cast<float>(M_PI)) + 0.5f;
	v = n.y * 0.5f + 0.5f;
}

glm::vec3 spherical_to_cartesian(float theta, float phi)
{
	float cos_phi = cosf(phi);
	float cos_theta = cosf(theta);
	float sin_phi = sinf(phi);
	float sin_theta = sinf(theta);
	return glm::vec3(
		cos_phi * sin_theta, sin_phi * sin_theta, cos_theta);
}

// Generates a random point with uniform probability distribution
//glm::vec3 random_on_unit_sphere(float (*f)(float) = [](float) -> float { return 1 / (4 * M_PI); })  // default for theta is uniform distribution
//{
//	/*
//	--- a and b are PDFs of theta and phi
//	a(theta) = 2 * PI * sin(theta) * f(theta)  // Depends on f, default is uniform
//	b(phi) = 1 / (2 * PI)  // Uniform
//
//	--- r1 and r2 two random numbers in [0, 1]
//	-- r1
//	r1 = INTEGRAL[0, theta](2 * PI * sin(t) f(t) * d(t))
//	if f(t) = 1 / 4PI (area of the sphere)
//	r1 = (1/2) * INTEGRAL[0, theta](sin(t) * d(t))
//	r1 = [cos(t)] = cos(theta)
//	r1 = cos(theta)
//	theta = arcos(r1) but we will actually just be using cos(theta)
//
//	-- r2
//	r2 = INTEGRAL[0, phi]((1 / (2 * PI)) d(p)) = phi / 2PI
//	phi = 2PI * r2
//	*/
//
//	// FIXME implement
//
//	return glm::vec3();
//}

glm::vec3 random_cosine_direction() {
	float r1 = frand();
	float r2 = frand();
	float phi = 2 * float(M_PI)* r1;
	float x = cos(phi) * 2 * sqrt(r2);
	float y = sin(phi) * 2 * sqrt(r2);
	float z = sqrtf(1 - r2);

	return glm::vec3(x, y, z);
}