#include "ModelLoader.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Triangle.h"
#include "ImageTexture.h"
#include "HitableList.h"

namespace {
    void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<HitableList> triangles);
    void processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<HitableList> triangles);
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<std::shared_ptr<Texture>>& textures);

    std::map<std::string, std::shared_ptr<Texture>> _texturesCache;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
}
    
std::shared_ptr<HitableList> ModelLoader::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::shared_ptr<HitableList> triangles = std::make_shared<HitableList>();
    processNode(scene->mRootNode, scene, triangles);

    return triangles;
}

namespace {

    void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<HitableList> triangles)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene, triangles);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, triangles);
        }

    }

    void processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<HitableList> triangles)
    {
        // Process material
        aiMaterial* assimp_material = scene->mMaterials[mesh->mMaterialIndex];

        std::shared_ptr<Material> material = std::make_shared<Material>();

        /* (Note from learnopengl since Itook the modifiedmodel from here:)
         We assume a convention for sampler names in the shaders. Each diffuse texture should be named
         as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
         Same applies to other texture as the following list summarizes:
         diffuse: texture_diffuseN
         specular: texture_specularN
         normal: texture_normalN
        */
        std::vector<std::shared_ptr<Texture>> diffuseMaps, specularMaps, normalMaps, heightMaps;
        loadMaterialTextures(assimp_material, aiTextureType_DIFFUSE, "texture_diffuse", diffuseMaps);
        loadMaterialTextures(assimp_material, aiTextureType_SPECULAR, "texture_specular", specularMaps);
        loadMaterialTextures(assimp_material, aiTextureType_HEIGHT, "texture_normal", normalMaps);
        loadMaterialTextures(assimp_material, aiTextureType_AMBIENT, "texture_height", heightMaps);
        if (diffuseMaps.size()) material->diffuse = diffuseMaps[0];
        if (specularMaps.size()) material->specular = specularMaps[0];
        if (normalMaps.size()) material->normal = normalMaps[0];
        if (heightMaps.size()) material->height = heightMaps[0];

        // Process triangles
        bool has_uv = mesh->mTextureCoords[0];
        bool has_normals = mesh->HasNormals();

        // Now walk through each of the mesh's faces' indices and retrieve the corresponding vertex data.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j += 3) {
                std::shared_ptr<Triangle> triangle = std::make_shared<Triangle>(
                    Vertex(  // Vertex 1
                        glm::vec3(mesh->mVertices[face.mIndices[j]].x, mesh->mVertices[face.mIndices[j]].y, mesh->mVertices[face.mIndices[j]].z),  // position
                        has_uv ? glm::vec2(mesh->mTextureCoords[0][face.mIndices[j]].x, mesh->mTextureCoords[0][face.mIndices[j]].y) : glm::vec2(0, 0),  // uv
                        has_normals ? glm::vec3(mesh->mNormals[face.mIndices[j]].x, mesh->mNormals[face.mIndices[j]].y, mesh->mNormals[face.mIndices[j]].z) : glm::vec3(1, 0, 0)  // normals
                    ),
                    Vertex(  // Vertex 2
                        glm::vec3(mesh->mVertices[face.mIndices[j + 1]].x, mesh->mVertices[face.mIndices[j + 1]].y, mesh->mVertices[face.mIndices[j + 1]].z),  // position
                        has_uv ? glm::vec2(mesh->mTextureCoords[0][face.mIndices[j + 1]].x, mesh->mTextureCoords[0][face.mIndices[j + 1]].y) : glm::vec2(0, 0),  // uv
                        has_normals ? glm::vec3(mesh->mNormals[face.mIndices[j + 1]].x, mesh->mNormals[face.mIndices[j + 1]].y, mesh->mNormals[face.mIndices[j + 1]].z) : glm::vec3(1, 0, 0)  // normals
                    ),
                    Vertex(  // Vertex 3
                        glm::vec3(mesh->mVertices[face.mIndices[j + 2]].x, mesh->mVertices[face.mIndices[j + 2]].y, mesh->mVertices[face.mIndices[j + 2]].z),  // position
                        has_uv ? glm::vec2(mesh->mTextureCoords[0][face.mIndices[j + 2]].x, mesh->mTextureCoords[0][face.mIndices[j + 2]].y) : glm::vec2(0, 0),  // uv
                        has_normals ? glm::vec3(mesh->mNormals[face.mIndices[j + 2]].x, mesh->mNormals[face.mIndices[j + 2]].y, mesh->mNormals[face.mIndices[j + 2]].z) : glm::vec3(1, 0, 0)  // normals
                    )
                    );
                triangle->material = material;
                triangles->list.push_back(triangle);
            }
        }
    }

    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<std::shared_ptr<Texture>>& textures)
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
                std::shared_ptr<ImageTexture> new_texture = std::make_shared<ImageTexture>("Survival_BackPack_2/" + texture_path);
                _texturesCache.insert(std::pair<std::string, std::shared_ptr<ImageTexture>>(texture_path, new_texture));
                textures.push_back(new_texture);
            }
        }
        return textures;
    }

}


