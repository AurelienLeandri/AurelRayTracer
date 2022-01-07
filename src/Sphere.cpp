#include "Sphere.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Transform.h"

Sphere::Sphere()
	: Shape(ShapeType::SPHERE)
{
}

Sphere::Sphere(const glm::vec3& center, float radius)
    : Shape(ShapeType::SPHERE)
{
	data.center = center;
	data.radius = radius;
}

float Sphere::area() const
{
    return 4 * (float)M_PI * radius * radius;
}

glm::vec3 Sphere::sample(const HitRecord& record, float& pdf) const
{
    return glm::vec3();
}

float Sphere::pdf(const glm::vec3& point, const HitRecord& record) const
{
    return 0.0f;
}

void Sphere::transform(const Transform& transform)
{
    center = glm::vec3(transform.getMatrix() * glm::vec4(center, 1));
}

void Sphere::commitGeometry(RTCDevice device, RTCScene rtcScene)
{
	RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_SPHERE_POINT);
	SphereData *bufferData = (SphereData *)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, sizeof(SphereData), 1);
	bufferData->center = data.center;
	bufferData->radius = data.radius;

	rtcCommitGeometry(geom);
	rtcAttachGeometry(rtcScene, geom);
	rtcReleaseGeometry(geom);
}
