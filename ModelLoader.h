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
class Mesh;
class Material;

class ModelLoader
{
public:
	static bool loadModel(std::string path, std::unordered_map<unsigned int, std::shared_ptr<Mesh>> &meshes, std::unordered_map<unsigned int, std::shared_ptr<Material>>& materials);

private:
    static void _processNode(
        aiNode* node,
        const aiScene* scene,
        std::unordered_map<unsigned int, std::shared_ptr<Material>>& global_materials,
        std::unordered_map<aiMaterial*, unsigned int>& model_materials,
        std::unordered_map<unsigned int, std::shared_ptr<Mesh>>& meshes,
        const std::string& directory);
    
    static void _processMesh(
        aiMesh* assimp_mesh,
        const aiScene* scene,
        std::unordered_map<unsigned int, std::shared_ptr<Material>>& global_materials,
        std::unordered_map<aiMaterial*, unsigned int>& model_materials,
        std::unordered_map<unsigned int, std::shared_ptr<Mesh>>& meshes,
        const std::string& directory);

private:
	static unsigned int _nextGeometryId;
	static unsigned int _nextMaterialId;
};

