#include "Scene.h"

#include "Embree.h"
#include "Shape.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Material.h"
#include "Light.h"
#include "Distribution2D.h"

std::vector<std::unique_ptr<Scene>> SceneFactory::_scenes;

Scene::~Scene()
{
}


const std::unordered_map<int, std::shared_ptr<Shape>>& Scene::getShapes() const
{
	return _geometries;
}

std::unordered_map<int, std::shared_ptr<Shape>>& Scene::getShapes()
{
	return _geometries;
}

const std::unordered_map<int, std::shared_ptr<Material>>& Scene::getMaterials() const
{
	return _materials;
}

std::unordered_map<int, std::shared_ptr<Material>>& Scene::getMaterials()
{
	return _materials;
}

const std::vector<std::shared_ptr<const Light>>& Scene::getLights() const
{
	return _lights;
}

const std::unordered_map<int, std::shared_ptr<const Light>>& Scene::getAreaLights() const
{
	return _areaLights;
}

std::vector<std::shared_ptr<const Light>>& Scene::getLights()
{
	return _lights;
}

const InfiniteAreaLight* Scene::getEnvironmentLight() const
{
    return _environmentLight;
}

int Scene::addShape(std::shared_ptr<Shape> shape)
{
    int lastIndex = static_cast<int>(_geometries.size());
	_geometries[lastIndex] = shape;
	return lastIndex;
}

int Scene::addLight(std::shared_ptr<Light> light, std::shared_ptr<Shape> lightShape)
{
    if (light->getType() == LightType::INFINITE_AREA) {
        _environmentLight = static_cast<InfiniteAreaLight*>(light.get());
    }
	_lights.push_back(light);
	if (lightShape) {
        int shapeId = static_cast<int>(_geometries.size());
        _geometries[shapeId] = lightShape;
        _areaLights[shapeId] = light;
		return shapeId;
	}
	return 0;
}

int Scene::addMaterial(std::shared_ptr<Material> material)
{
    int lastIndex = static_cast<int>(_materials.size()) + 1;
	_materials[lastIndex] = material;
	return lastIndex;
}

Scene *SceneFactory::createScene()
{
	_scenes.push_back(std::make_unique<Scene>());
	return _scenes.back().get();
}
