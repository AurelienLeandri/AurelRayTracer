#include "Camera.h"

#include "ray.h"
#include "Utils.h"

#include <math.h>
#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 look_at, glm::vec3 up_axis, float fov, float aspect, float aperture, float focus_dist, float t0, float t1) :
	_position(position), _aperture(aperture), _lensRadius(aperture / 2.f), _focusDist(focus_dist), _t0(std::min(t0, t1)), _t1(std::max(t0, t1))
{
	// Compute coordinate system
	_front = glm::normalize(look_at - position);
	_right = glm::normalize(glm::cross(_front, glm::normalize(up_axis)));
	_up = glm::normalize(cross(_right, _front));

	// Screen
	// (NOTE: fov is the vertical angle), focus dist is the distance between the position and the screen
	float height = tan(fov / 2.f) * _focusDist * 2.f;  // Middle-school trigonometry to get the height of the screen from the focus distance and tan
	_screenDimensions = glm::vec2(height * aspect, height);
	_lowerLeftCorner = _position - 0.5f * _screenDimensions.x * _right - 0.5f * _screenDimensions.y * _up + _front * _focusDist;
}

Ray Camera::getRay(float x, float y)
{
	glm::vec3 random = _lensRadius * random_in_unit_disc();
	glm::vec3 offset(0.f, 0.f, 0.f);
	return Ray(_position + offset, _lowerLeftCorner + x * _screenDimensions.x * _right + y * _screenDimensions.y * _up - _position - offset);
}

std::shared_ptr<const Film> Camera::getFilm() const
{
	return _film;
}

std::shared_ptr<Film> Camera::getFilm()
{
	return _film;
}

float Camera::generateRayDifferential(const CameraSample& sample, RayDifferential& ray) const
{
	return 1.0f;
}
