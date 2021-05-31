#pragma once

#include <unordered_map>
#include <memory>

class Material;
class Shape;
class Light;

/*
* Holds the data of every mesh throughout application lifetime
*/
class SceneData
{
public:
	const std::unordered_map<unsigned int, std::shared_ptr<Shape>>& getShapes() const;
	std::unordered_map<unsigned int, std::shared_ptr<Shape>>& getShapes();
	const std::unordered_map<unsigned int, std::shared_ptr<Material>>& getMaterials() const;
	std::unordered_map<unsigned int, std::shared_ptr<Material>>& getMaterials();
	const std::vector<std::shared_ptr<Light>>& getLights() const;
	std::vector<std::shared_ptr<Light>>& getLights();

	unsigned int addShape(std::shared_ptr<Shape> mesh);

	// Add a light to the scene, along with its optional shape. Returns the id of the shape if present, 0 else.
	unsigned int addLight(std::shared_ptr<Light> light, std::shared_ptr<Shape> lightShape = nullptr);

	unsigned int addMaterial(std::shared_ptr<Material> material);

private:
	std::unordered_map<unsigned int, std::shared_ptr<Shape>> _geometries;
	std::unordered_map<unsigned int, std::shared_ptr<Material>> _materials;
	std::vector<std::shared_ptr<Light>> _lights;
};

class SceneFactory {
public:
	static SceneData *createScene();
private:
	static std::vector<SceneData> _scenes;
};

