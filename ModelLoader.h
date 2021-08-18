#pragma once

#include <vector>
#include <memory>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <unordered_set>

#include "Transform.h"

class Material;
class Scene;
class ImageTexture;

/**
 * Provides functions to load models from a file into a scene. This class uses the assimp library for model loading.
 */
/* static */ class ModelLoader
{
public:
    /**
     * Loads a model from a file into a scene. Can apply a global transform to it as well.
     * 
     * \param path The path to the file from which to load the model.
     * \param scene The scene to add the model to once it has been loaded.
     * \param transform A global transform applied to the whole model on load. Must be well-formed.
     * \return An error code describing the result of the model loading.
     */
    static int loadModel(std::string path, Scene& scene, const Transform& transform = {});

private:
    /**
     * Processes a node into the assimp scene graph created from the model. It is recursively called on
     * its children and loads meshes using _processMesh when encountering one.
     * 
     * \param node The current assimp node
     * \param ai_scene The scene representation in Assimp to fetch data from.
     * \param model_materials A cache for all materials in the model that were already loaded, to avoid duplicates. Will be filled as we progress down the scene graph.
     * \param directory Directory of the model file; used to find the associated textures in _loadMaterialTextures
     * \param scene The scene to add the mesh to.
     * \param transform The global transform to apply to the model.
     */
    static void _processNode(
        aiNode* node,
        const aiScene* aiScene,
        std::unordered_map<aiMaterial*, int>& modelMaterialsCache,
        const std::string& directory, Scene& scene, const Transform& transform);
    
    /**
     * Processes a mesh in the model and adds it to the Scene, along with its materials.
     * 
     * \param assimp_mesh The Assimp representation of the mesh.
     * \param ai_scene The Assimp representation of the scene.
     * \param model_materials A cache for all materials in the model that were already loaded, to avoid duplicates. Will be filled as we progress down the scene graph.
     * \param directory Directory of the model file; used to find the associated textures in _loadMaterialTextures
     * \param scene The scene to add the mesh to.
     * \param transform The global transform to apply to the model.
     */
    static void _processMesh(
        aiMesh* assimpMesh,
        const aiScene* aiScene,
        std::unordered_map<aiMaterial*, int>& modelMaterialsCache,
        const std::string& directory, Scene& scene, const Transform& transform);

    static std::shared_ptr<Material> _loadMaterial(
        aiMaterial* assimpMaterial,
        const std::string& directory
    );

    /**
     * Loads the texture of a given type from a material.
     * Note: There can technically be several textures of a given type
     * on an Assimp material, however we ignore this case and load only
     * the first one (i.e. at index 0)
     * 
     * \param mat The Assimp representation of the material.
     * \param type The texture type as defined in Assimp.
     * \param directory The directory from which to load the texture.
     * \param textures The vector of Texture objects to populate with the loaded textures.
     */
    static std::shared_ptr<ImageTexture> _loadMaterialTexture(
        aiMaterial* assimpMaterial,
        aiTextureType assimpTextureType,
        const std::string& directory);

private:
    /**
     * Stores all the textures loaded so far to make sure textures aren't loaded more than once.
     */
    static std::unordered_map<std::string, std::shared_ptr<ImageTexture>> _texturesCache;
};

