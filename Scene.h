#pragma once

#include <unordered_map>
#include <memory>

class Material;
class Shape;
class Light;
class InfiniteAreaLight;

/**
 * Holds the data of a scene and provides an interface for accessing and modifying it.
 */
class Scene
{
// Attributes
private:
	// Data owned by the scene
	/**
	 * The shapes registered in the scene, identified by their unique id.
	 */
	std::unordered_map<int, std::shared_ptr<Shape>> _shapes;

	/**
	 * The materials registered in the scene, identified by their unique id.
	 */
	std::unordered_map<int, std::shared_ptr<Material>> _materials;

	/**
	 * The area lights of the scene, identified by the id of their associated shape.
	 */
	std::unordered_map<int, std::shared_ptr<const Light>> _areaLights;

	/**
	 * The lights of the scene.
	 */
	std::vector<std::shared_ptr<const Light>> _lights;

	/**
	 * The environment light of the scene, if any.
	 */
	const InfiniteAreaLight* _environmentLight = nullptr;

// Populating the scene
public:
	/**
	 * Adds a shape of arbitrary type to the scene and returns the unique id assigned to that shape after being added.
	 * 
	 * \param shape The shape to add.
	 * \return The newly-assigned unique id of the shape in the scene.
	 */
	int addShape(std::shared_ptr<Shape> shape);

	/**
	 * Adds a light of arbitrary type to the scene. If lightShape is provided, adds an associated shape for the light
	 * (for example in the case of an AreaLight). Returns the lightShape's id if lightShape was provided, 0 else.
	 * If light is of type InfiniteAreaLight, it will replace the environment light currently attached to the scene, if any.
	 * 
	 * \param light The light to add.
	 * \param lightShape The shape associated to the light, if any (for example in the case of an AreaLight).
	 * \return The newly-assigned unique id of lightShape after being added. If lightShape was not provided, 0 is returned.
	 */
	int addLight(std::shared_ptr<Light> light, std::shared_ptr<Shape> lightShape = nullptr);

	/**
	 * Adds a material of arbitrary type to the scene and returns the unique id assigned to it after being added.
	 * 
	 * \param material The material to add.
	 * \return The newly-assigned unique id of the material after being added.
	 */
	int addMaterial(std::shared_ptr<Material> material);

// Accessors
public:
	const std::unordered_map<int, std::shared_ptr<Shape>>& getShapes() const;
	std::unordered_map<int, std::shared_ptr<Shape>>& getShapes();
	const std::unordered_map<int, std::shared_ptr<Material>>& getMaterials() const;
	std::unordered_map<int, std::shared_ptr<Material>>& getMaterials();
	const std::vector<std::shared_ptr<const Light>>& getLights() const;
	std::vector<std::shared_ptr<const Light>>& getLights();
	const std::unordered_map<int, std::shared_ptr<const Light>>& getAreaLights() const;
	const InfiniteAreaLight* getEnvironmentLight() const;
};

/**
 * Static data manager for all scenes. Scenes cannot be created directly and must be via this class.
 */
/* static */ class SceneFactory {
public:
	/**
	 * Creates a scene and holds its data.
	 * 
	 * \return The address of the newly-created scene.
	 */
	static Scene *createScene();

// Data holder for the scenes
private:
	/**
	 * Data holder for all created scenes to ensure they are kept throughout runtime.
	 */
	static std::vector<std::unique_ptr<Scene>> _scenes;
};

