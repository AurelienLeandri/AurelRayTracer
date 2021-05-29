#include "ModelLoader.h"

#include <glm/glm.hpp>
#include <stb_image.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "Triangle.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "SceneData.h"

namespace {
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<std::shared_ptr<Texture>>& textures, const std::string &directory);
    std::unordered_map<std::string, std::shared_ptr<Texture>> _texturesCache;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
}
   
bool ModelLoader::loadModel(std::string path, SceneData& scene, const Transform& transform)
{
    Assimp::Importer importer;
    const aiScene* ai_scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_SortByPType);
    if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) // if is Not Zero
    {
        std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    std::unordered_map<aiMaterial*, unsigned int> model_materials;
    std::string directory = path.substr(0, path.find_last_of('/'));
    _processNode(ai_scene->mRootNode, ai_scene, model_materials, directory, scene, transform);

    return true;
}


void ModelLoader::_processNode(
    aiNode* node,
    const aiScene* ai_scene,
    std::unordered_map<aiMaterial*, unsigned int>& model_materials,
    const std::string& directory, SceneData& scene, const Transform& transform)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = ai_scene->mMeshes[node->mMeshes[i]];
        _processMesh(mesh, ai_scene, model_materials, directory, scene, transform);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        _processNode(node->mChildren[i], ai_scene, model_materials, directory, scene, transform);
    }

}

void ModelLoader::_processMesh(
    aiMesh* assimp_mesh,
    const aiScene* ai_scene,
    std::unordered_map<aiMaterial*, unsigned int>& model_materials,
    const std::string& directory, SceneData& scene, const Transform& transform)
{
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    // Process material
    aiMaterial* assimp_material = ai_scene->mMaterials[assimp_mesh->mMaterialIndex];

    if (assimp_material && model_materials.find(assimp_material) == model_materials.end()) {
        std::shared_ptr<MatteMaterial> material;

        /* (Note from learnopengl since Itook the modifiedmodel from here:)
            We assume a convention for sampler names in the shaders. Each diffuse texture should be named
            as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
            Same applies to other texture as the following list summarizes:
            diffuse: texture_diffuseN
            specular: texture_specularN
            normal: texture_normalN
        */
        std::vector<std::shared_ptr<Texture>> diffuseMaps, specularMaps, normalMaps, heightMaps;
        loadMaterialTextures(assimp_material, aiTextureType_DIFFUSE, "texture_diffuse", diffuseMaps, directory);
        loadMaterialTextures(assimp_material, aiTextureType_SPECULAR, "texture_specular", specularMaps, directory);
        loadMaterialTextures(assimp_material, aiTextureType_HEIGHT, "texture_normal", normalMaps, directory);
        loadMaterialTextures(assimp_material, aiTextureType_AMBIENT, "texture_height", heightMaps, directory);
        if (diffuseMaps.size()) material = std::make_shared<MatteMaterial>(diffuseMaps[0]);
        else {
            material = std::make_shared<MatteMaterial>(ConstantTexture::white);
        }
        /*
        if (specularMaps.size()) material->specular = specularMaps[0];
        if (normalMaps.size()) material->normal = normalMaps[0];
        if (heightMaps.size()) material->height = heightMaps[0];
        */

        model_materials[assimp_material] = scene.addMaterial(material);
    }

    mesh->materialId = model_materials[assimp_material];

    // Process triangles
    bool has_uv = assimp_mesh->mTextureCoords[0];
    bool has_normals = assimp_mesh->HasNormals();

    for (unsigned int i = 0; i < assimp_mesh->mNumFaces; i++)
    {
        const aiFace& face = assimp_mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            mesh->indices.push_back(face.mIndices[j]);
        }

    }

    for (unsigned int i = 0; i < assimp_mesh->mNumVertices; i++) {
        auto a = assimp_mesh->mVertices[i];
        auto c = assimp_mesh->mNormals[i];
        mesh->geometry.push_back({
                glm::vec3(assimp_mesh->mVertices[i].x, assimp_mesh->mVertices[i].y, assimp_mesh->mVertices[i].z),
                has_normals ? glm::vec3(assimp_mesh->mNormals[i].x, assimp_mesh->mNormals[i].y, assimp_mesh->mNormals[i].z) : glm::vec3(1, 0, 0),
                has_uv ? glm::vec2(assimp_mesh->mTextureCoords[0][i].x, assimp_mesh->mTextureCoords[0][i].y) : glm::vec2(0, 0)
            });
    }

    mesh->transform(transform);

    scene.addShape(mesh);
}

namespace {
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<std::shared_ptr<Texture>>& textures, const std::string& directory)
    {
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string texture_path = str.C_Str();

            if (_texturesCache.find(texture_path) != _texturesCache.end()) {
                textures.push_back(_texturesCache[texture_path]);
            }
            else {
                std::shared_ptr<ImageTexture> new_texture = std::make_shared<ImageTexture>(directory + "/" + texture_path);
                _texturesCache.insert(std::pair<std::string, std::shared_ptr<ImageTexture>>(texture_path, new_texture));
                textures.push_back(new_texture);
            }
        }
        return textures;
    }

}


