#include "SceneData.h"

std::vector<SceneData> SceneFactory::_scenes;

const std::unordered_map<unsigned int, std::shared_ptr<Mesh>>& SceneData::getMeshes() const
{
	return _geometries;
}

std::unordered_map<unsigned int, std::shared_ptr<Mesh>>& SceneData::getMeshes()
{
	return _geometries;
}

const std::unordered_map<unsigned int, std::shared_ptr<Material>>& SceneData::getMaterials() const
{
	return _materials;
}

std::unordered_map<unsigned int, std::shared_ptr<Material>>& SceneData::getMaterials()
{
	return _materials;
}

unsigned int SceneData::addMesh(std::shared_ptr<Mesh> mesh)
{
	_geometries[_geometries.size()] = mesh;
	return _geometries.size() - 1;
}

unsigned int SceneData::addMaterial(std::shared_ptr<Material> material)
{
	_materials[_materials.size() + 1] = material;
	return _materials.size();
}

SceneData *SceneFactory::createScene()
{
	_scenes.push_back(SceneData());
	return &_scenes.back();
}
