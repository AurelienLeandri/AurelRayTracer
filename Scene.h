#pragma once

#include <unordered_map>
#include <memory>
#include <embree3/rtcore.h>

class Material;
class Shape;
class Light;
class Ray;
struct HitRecord;
class InfiniteAreaLight;

/*
* Holds the data of every mesh throughout ApplicationOld lifetime
*/
class Scene
{
public:
	~Scene();

public:
	bool computeAccelerationStructures();
	bool castRay(const Ray& ray, HitRecord& hit_record) const;

public:
	const std::unordered_map<int, std::shared_ptr<Shape>>& getShapes() const;
	std::unordered_map<int, std::shared_ptr<Shape>>& getShapes();
	const std::unordered_map<int, std::shared_ptr<Material>>& getMaterials() const;
	std::unordered_map<int, std::shared_ptr<Material>>& getMaterials();
	const std::vector<std::shared_ptr<const Light>>& getLights() const;
	std::vector<std::shared_ptr<const Light>>& getLights();
	const InfiniteAreaLight* getEnvironmentLight() const;

	int addShape(std::shared_ptr<Shape> mesh);

	// Add a light to the scene, along with its optional shape. Returns the id of the shape if present, 0 else.
	int addLight(std::shared_ptr<Light> light, std::shared_ptr<Shape> lightShape = nullptr);

	int addMaterial(std::shared_ptr<Material> material);

private:
	std::unordered_map<int, std::shared_ptr<Shape>> _geometries;  // Keys are shape ids
	std::unordered_map<int, std::shared_ptr<Material>> _materials;  // Keys are material ids
	std::unordered_map<int, std::shared_ptr<const Light>> _areaLights;  // Keys are associated shapes
	std::vector<std::shared_ptr<const Light>> _lights;
	InfiniteAreaLight *_environmentLight = nullptr;

	// Handlers for Embree
	RTCScene _rtcScene = 0;
	RTCDevice _rtcDevice = 0;
};

class SceneFactory {
public:
	static Scene *createScene();
private:
	static std::vector<std::unique_ptr<Scene>> _scenes;
};

