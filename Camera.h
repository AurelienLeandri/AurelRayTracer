#pragma once

#include <glm/glm.hpp>
#include <memory>

class Ray;
class Film;
struct CameraSample;
class RayDifferential;

struct CameraSample {
	glm::vec2 pFilm;
	glm::vec2 pLens;
	float time;
};

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 look_at, glm::vec3 up_axis, float fov, float aspect, float aperture = 1.f, float focus_dist = 1.f, float t0 = 0.f, float t1 = 0.f);

	Ray getRay(float x, float y) const;

public:
	std::shared_ptr<const Film> getFilm() const;
	std::shared_ptr<Film> getFilm();
	float generateRayDifferential(const CameraSample& sample, RayDifferential& ray) const;
	const glm::vec3& getPosition() const;
	const glm::vec3& getUp() const;
	const glm::vec3& getRight() const;
	const glm::vec3& getFront() const;
	void setPosition(const glm::vec3& position);
	void setFront(const glm::vec3& front);

private:
	// Screen informations
	glm::vec2 _screenDimensions;
	glm::vec3 _lowerLeftCorner;

	// Camera position and cartesian coordinates system
	glm::vec3 _position;
	glm::vec3 _up;
	glm::vec3 _right;
	glm::vec3 _front;

	// Zoom
	float _aperture;  // diameter of the pinhole
	float _focusDist;

	// Focus blur
	float _lensRadius;  // radius of the pinhole

	// Exposition time for motion blur
	float _t0;
	float _t1;

	std::shared_ptr<Film> _film;
};

