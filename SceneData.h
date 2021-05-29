#pragma once

#include <unordered_map>
#include <memory>

class Material;
class Shape;

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
	unsigned int addShape(std::shared_ptr<Shape> mesh);
	unsigned int addMaterial(std::shared_ptr<Material> material);

private:
	std::unordered_map<unsigned int, std::shared_ptr<Shape>> _geometries;
	std::unordered_map<unsigned int, std::shared_ptr<Material>> _materials;
};

class SceneFactory {
public:
	static SceneData *createScene();
private:
	static std::vector<SceneData> _scenes;
};

