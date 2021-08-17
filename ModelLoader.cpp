#include "ModelLoader.h"

#include <glm/glm.hpp>
#include <stb_image.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "Triangle.h"
#include "ImageTexture.h"
#include "Mesh.h"
#include "Material.h"
#include "Scene.h"

std::unordered_map<std::string, std::shared_ptr<ImageTexture>> ModelLoader::_texturesCache;
   
int ModelLoader::loadModel(std::string path, Scene& scene, const Transform& transform)
{
    Assimp::Importer importer;
    const aiScene* aiScene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_SortByPType
    );

    if (!aiScene || (aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !aiScene->mRootNode)
    {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return -1;
    }

    std::unordered_map<aiMaterial*, int> modelMaterials;
    std::string directory = path.substr(0, path.find_last_of('/'));
    _processNode(aiScene->mRootNode, aiScene, modelMaterials, directory, scene, transform);

    return 0;
}


void ModelLoader::_processNode(
    aiNode* node,
    const aiScene* aiScene,
    std::unordered_map<aiMaterial*, int>& modelMaterialsCache,
    const std::string& directory, Scene& scene, const Transform& transform)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        _processMesh(aiScene->mMeshes[node->mMeshes[i]], aiScene, modelMaterialsCache, directory, scene, transform);
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        _processNode(node->mChildren[i], aiScene, modelMaterialsCache, directory, scene, transform);
    }

}

void ModelLoader::_processMesh(
    aiMesh* assimpMesh,
    const aiScene* aiScene,
    std::unordered_map<aiMaterial*, int>& modelMaterialsCache,
    const std::string& directory, Scene& scene, const Transform& transform)
{
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    // Process material
    aiMaterial* assimpMaterial = aiScene->mMaterials[assimpMesh->mMaterialIndex];

    // There is a material attached to the mesh
    if (assimpMaterial) {
        if (modelMaterialsCache.find(assimpMaterial) == modelMaterialsCache.end()) {  // First time seeing the material
            // Creating the material
            std::shared_ptr<Material> material = _loadMaterial(assimpMaterial, directory);

            // Add the material to the Scene and also the cache to avoid duplicates.
            modelMaterialsCache[assimpMaterial] = scene.addMaterial(material);
        }
        mesh->setMaterialId(modelMaterialsCache[assimpMaterial]);
    }

    // Populate indices vector
    std::vector<int>& indices = mesh->getIndices();
    for (unsigned int i = 0; i < assimpMesh->mNumFaces; ++i)
    {
        const aiFace& face = assimpMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Populate vertices vector
    std::vector<Vertex>& vertices = mesh->getVertices();
    bool hasUv = assimpMesh->mTextureCoords[0];
    bool hasNormals = assimpMesh->HasNormals();
    for (unsigned int i = 0; i < assimpMesh->mNumVertices; ++i) {
        vertices.push_back({
                glm::vec3(assimpMesh->mVertices[i].x, assimpMesh->mVertices[i].y, assimpMesh->mVertices[i].z),  // Position
                hasNormals ? glm::vec3(assimpMesh->mNormals[i].x, assimpMesh->mNormals[i].y, assimpMesh->mNormals[i].z) : glm::vec3(0, 0, 1),  // Normal or z+ by default
                hasUv ? glm::vec2(assimpMesh->mTextureCoords[0][i].x, assimpMesh->mTextureCoords[0][i].y) : glm::vec2(0, 0)  // UVs if any
            });
    }

    // Apply global transform to the mesh
    mesh->transform(transform);

    scene.addShape(mesh);
}

std::shared_ptr<Material> ModelLoader::_loadMaterial(aiMaterial* assimpMaterial, const std::string& directory)
{
    // TODO: Check which material should be created using what values and textures are present.
    std::shared_ptr<MatteMaterial> material = std::make_shared<MatteMaterial>();
    aiTextureType textureTypes[] = {
        aiTextureType_DIFFUSE
    };
    std::unordered_map<aiTextureType, std::shared_ptr<Texture>&> materialTextureSlots = {
        { aiTextureType_DIFFUSE, material->albedo }
    };
    for (const aiTextureType& textureType : textureTypes) {
        std::shared_ptr<ImageTexture> texture = _loadMaterialTexture(assimpMaterial, textureType, directory);
        if (texture) {
            materialTextureSlots[textureType] = texture;
        }
        // TODO: else case, for example adding a constant texture from another parameter of the material
    }
}

std::shared_ptr<ImageTexture> ModelLoader::_loadMaterialTexture(
    aiMaterial* assimpMaterial,
    aiTextureType assimpTextureType,
    const std::string& directory)
{
    if (!assimpMaterial->GetTextureCount(assimpTextureType)) {
        return nullptr;
    }

    aiString str;
    assimpMaterial->GetTexture(assimpTextureType, 0, &str);  // "0" for texture at index 0. The rest is ignored because unsupported by all renderers.
    std::string texturePath = str.C_Str();

    std::shared_ptr<ImageTexture> texture;
    if (_texturesCache.find(texturePath) != _texturesCache.end()) {
        texture = _texturesCache[texturePath];  // Texture was already in the cache so we get it from there.
    }
    else {
        texture = std::make_shared<ImageTexture>(directory + "/" + texturePath);
        _texturesCache.insert(std::pair<std::string, std::shared_ptr<ImageTexture>>(texturePath, texture));
    }
    return texture;
}

