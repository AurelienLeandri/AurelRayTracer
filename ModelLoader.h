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

class Texture;
class Material;
class SceneData;

class ModelLoader
{
public:
    static bool loadModel(std::string path, SceneData& scene, const Transform& transform = {});

private:
    static void _processNode(
        aiNode* node,
        const aiScene* ai_scene,
        std::unordered_map<aiMaterial*, unsigned int>& model_materials,
        const std::string& directory, SceneData& scene, const Transform& transform);
    
    static void _processMesh(
        aiMesh* assimp_mesh,
        const aiScene* ai_scene,
        std::unordered_map<aiMaterial*, unsigned int>& model_materials,
        const std::string& directory, SceneData& scene, const Transform& transform);
};

