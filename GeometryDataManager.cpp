#include "SceneData.h"

std::vector<SceneData> SceneFactory::_scenes;

const std::unordered_map<unsigned int, std::shared_ptr<Shape>>& SceneData::getShapes() const
{
	return _geometries;
}

std::unordered_map<unsigned int, std::shared_ptr<Shape>>& SceneData::getShapes()
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

const std::vector<std::shared_ptr<Light>>& SceneData::getLights() const
{
	return _lights;
}

std::vector<std::shared_ptr<Light>>& SceneData::getLights()
{
	return _lights;
}

unsigned int SceneData::addShape(std::shared_ptr<Shape> shape)
{
	_geometries[_geometries.size()] = shape;
	return _geometries.size() - 1;
}

unsigned int SceneData::addLight(std::shared_ptr<Light> light, std::shared_ptr<Shape> lightShape)
{
	_lights.push_back(light);
	if (lightShape) {
		_geometries[_geometries.size()] = lightShape;
		return _geometries.size() - 1;
	}
	return 0;
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
