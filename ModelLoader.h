#pragma once

#include <vector>
#include <memory>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <unordered_set>

class HitableList;
class Texture;
class Material;
class SceneData;

class ModelLoader
{
public:
    static bool loadModel(std::string path, SceneData &scene);

private:
    static void _processNode(
        aiNode* node,
        const aiScene* ai_scene,
        std::unordered_map<aiMaterial*, unsigned int>& model_materials,
        const std::string& directory, SceneData& scene);
    
    static void _processMesh(
        aiMesh* assimp_mesh,
        const aiScene* ai_scene,
        std::unordered_map<aiMaterial*, unsigned int>& model_materials,
        const std::string& directory, SceneData& scene);
};

