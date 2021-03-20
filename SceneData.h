#pragma once

#include <unordered_map>
#include <memory>

class Mesh;
class Material;

/*
* Holds the data of every mesh throughout application lifetime
*/
class SceneData
{
public:
	const std::unordered_map<unsigned int, std::shared_ptr<Mesh>>& getMeshes() const;
	const std::unordered_map<unsigned int, std::shared_ptr<Material>>& getMaterials() const;
	unsigned int addMesh(std::shared_ptr<Mesh> mesh);
	unsigned int addMaterial(std::shared_ptr<Material> material);

private:
	std::unordered_map<unsigned int, std::shared_ptr<Mesh>> _geometries;
	std::unordered_map<unsigned int, std::shared_ptr<Material>> _materials;
};

class SceneFactory {
public:
	static SceneData *createScene();
private:
	static std::vector<SceneData> _scenes;
};

