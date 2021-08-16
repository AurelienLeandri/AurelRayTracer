#pragma once

#include <embree3/rtcore.h>

struct Vertex;
struct HitRecord;
class Scene;
class Ray;
class Shape;
class Triangle;
class Sphere;
class Mesh;

class Embree
{
// Attributes
private:
	RTCDevice _rtcDevice = 0;
	RTCScene _rtcScene = 0;
	const Scene* _scene = nullptr;

// Constructors/Destructors
public:
	~Embree();
private:
	Embree();
	void _cleanupScene();

// Singleton
public:
	static Embree* getInstance();
private:
	static Embree* _instance;

// Scene setup
public:
	void setScene(const Scene& scene);
private:
	void _addShape(const Shape* shape);
	void _addTriangle(const Triangle* triangle);
	void _addSphere(const Sphere* sphere);
	void _addMesh(const Mesh* mesh);

// Intersection
public:
	bool castRay(const Ray& ray, HitRecord& hit_record) const;

};

