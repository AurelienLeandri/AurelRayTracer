#include "ResourceManager.h"

#include <sstream>

namespace {
    bool isIdValid(std::string id);
}

/*
* ImageTextureManager
*/

int ImageTextureManager::loadFromFile(std::string fileName, ImageTexture*& texture, std::string* id)
{
    if (!getFromFileName(fileName, texture)) {
        if (id) {
            auto idFound = _fileNameToId.find(fileName);
            *id = idFound != _fileNameToId.end() ? idFound->second : "_" + fileName;
        }
        return 1;
    }

    // TODO: create texture from file

    std::string textureId = "_" + fileName;
    _texturesMap[textureId] = std::unique_ptr<ImageTexture>(texture);
    if (id) {
        *id = textureId;
    }

    return 0;
}

int ImageTextureManager::loadFromFile(std::string id, std::string fileName, ImageTexture*& texture)
{
    if (!isIdValid(id)) {
        return -1;
    }

    if (!get(id, texture)) {
        return 1;
    }

    // TODO: create texture from file

    _texturesMap[id] = std::unique_ptr<ImageTexture>(texture);
    _fileNameToId[fileName] = id;

    return 0;
}

int ImageTextureManager::add(std::string id, ImageTexture* texture)
{
    if (!texture || !isIdValid(id)) {
        return -1;
    }

    ImageTexture* alreadyThere = nullptr;
    if (!get(id, alreadyThere)) {
        if (alreadyThere != texture) {
            return -2;
        }
        return 1;
    }

    _texturesMap[id] = std::unique_ptr<ImageTexture>(texture);

    return 0;
}

int ImageTextureManager::add(ImageTexture* texture, std::string* id)
{
    if (!texture) {
        return -1;
    }

    std::stringstream stream;
    stream << "_" << static_cast<void*>(texture);
    std::string textureId = stream.str();

    _texturesMap[textureId] = std::unique_ptr<ImageTexture>(texture);
    if (id) {
        *id = textureId;
    }

    return 0;
}

int ImageTextureManager::addOrReplace(std::string id, ImageTexture* texture)
{
    if (!texture || !isIdValid(id)) {
        return -1;
    }

    _texturesMap.erase(id);

    _texturesMap[id] = std::unique_ptr<ImageTexture>(texture);

    return 0;
}

int ImageTextureManager::create(std::string id, size_t width, size_t height, size_t nbChannels, ImageTexture*& texture)
{
    if (!isIdValid(id)) {
        return -1;
    }

    if (!get(id, texture)) {
        if (texture->getWidth() != width || texture->getHeight() != height || texture->getNbChannels() != nbChannels) {
            return -2;
        }
        return 1;
    }

    // TODO: create texture from scratch

    _texturesMap[id] = std::unique_ptr<ImageTexture>(texture);

    return 0;
}

int ImageTextureManager::create(size_t width, size_t height, size_t nbChannels, ImageTexture*& texture, std::string* id)
{
    // TODO: create texture from scratch

    std::stringstream stream;
    stream << "_" << static_cast<void*>(texture);
    std::string textureId = stream.str();

    _texturesMap[textureId] = std::unique_ptr<ImageTexture>(texture);
    if (id) {
        *id = textureId;
    }

    return 0;
}

int ImageTextureManager::createOrReplace(std::string id, size_t width, size_t height, size_t nbChannels, ImageTexture*& texture)
{
    if (!isIdValid(id)) {
        return -1;
    }

    _texturesMap.erase(id);

    // TODO: create texture from scratch

    _texturesMap[id] = std::unique_ptr<ImageTexture>(texture);

    return 0;
}

int ImageTextureManager::get(std::string id, ImageTexture*& texture)
{
    auto found = _texturesMap.find(id);
    if (found != _texturesMap.end()) {
        texture = found->second.get();
        return 0;
    }
    return -1;
}

int ImageTextureManager::getFromFileName(std::string fileName, ImageTexture*& texture)
{
    auto idFound = _fileNameToId.find(fileName);
    if (idFound != _fileNameToId.end()) {
        return get(idFound->second, texture);
    }
    return get("_" + fileName, texture);
}

/*
* MeshManager
*/

int MeshManager::loadFromFile(std::string fileName, std::vector<Mesh*>& meshes, std::string* id)
{
    if (!getFromFileName(fileName, meshes)) {
        if (id) {
            auto idFound = _fileNameToId.find(fileName);
            *id = idFound != _fileNameToId.end() ? idFound->second : "_" + fileName;
        }
        return 1;
    }

    // TODO: create meshes from file

    std::string meshId = "_" + fileName;
    _meshesMap[meshId] = std::vector<std::unique_ptr<Mesh>>();
    for (Mesh* mesh : meshes) {
        _meshesMap[meshId].push_back(std::unique_ptr<Mesh>(mesh));
    }
    if (id) {
        *id = meshId;
    }

    return 0;
}

int MeshManager::loadFromFile(std::string id, std::string fileName, std::vector<Mesh*>& meshes)
{
    if (!isIdValid(id)) {
        return -1;
    }

    if (!get(id, meshes)) {
        return 1;
    }

    // TODO: create mesh from file

    _meshesMap[id] = std::vector<std::unique_ptr<Mesh>>();
    for (Mesh* mesh : meshes) {
        _meshesMap[id].push_back(std::unique_ptr<Mesh>(mesh));
    }
    _fileNameToId[fileName] = id;

    return 0;
}

int MeshManager::add(std::string id, const std::vector<Mesh*>& meshes)
{
    if (meshes.size() == 0 || !isIdValid(id)) {
        return -1;
    }

    std::vector<Mesh*> alreadyThere;
    if (!get(id, alreadyThere)) {
        if (meshes.size() != alreadyThere.size() || meshes != alreadyThere) {
            return -2;
        }
        return 1;
    }

    _meshesMap[id] = std::vector<std::unique_ptr<Mesh>>();
    for (Mesh* mesh : meshes) {
        _meshesMap[id].push_back(std::unique_ptr<Mesh>(mesh));
    }

    return 0;
}

int MeshManager::add(const std::vector<Mesh*>& meshes, std::string* id)
{
    if (meshes.size() == 0) {
        return -1;
    }

    std::stringstream stream;
    stream << "_" << static_cast<void*>(meshes[0]);
    std::string meshId = stream.str();

    _meshesMap[meshId] = std::vector<std::unique_ptr<Mesh>>();
    for (Mesh* mesh : meshes) {
        _meshesMap[meshId].push_back(std::unique_ptr<Mesh>(mesh));
    }
    if (id) {
        *id = meshId;
    }

    return 0;
}

int MeshManager::addOrReplace(std::string id, const std::vector<Mesh*>& meshes)
{
    if (meshes.size() == 0 || !isIdValid(id)) {
        return -1;
    }

    _meshesMap.erase(id);

    _meshesMap[id] = std::vector<std::unique_ptr<Mesh>>();
    for (Mesh* mesh : meshes) {
        _meshesMap[id].push_back(std::unique_ptr<Mesh>(mesh));
    }

    return 0;
}

int MeshManager::create(std::string id, size_t nbMeshes, std::vector<Mesh*>& meshes)
{
    if (!isIdValid(id) || nbMeshes == 0) {
        return -1;
    }

    if (!get(id, meshes)) {
        if (meshes.size() != nbMeshes) {
            return -2;
        }
        return 1;
    }

    meshes = std::vector<Mesh*>(nbMeshes, new Mesh());

    _meshesMap[id] = std::vector<std::unique_ptr<Mesh>>();
    for (Mesh* mesh : meshes) {
        _meshesMap[id].push_back(std::unique_ptr<Mesh>(mesh));
    }

    return 0;
}

int MeshManager::create(size_t nbMeshes, std::vector<Mesh*>& meshes, std::string* id)
{
    if (nbMeshes == 0)
        return -1;

    meshes = std::vector<Mesh*>(nbMeshes, new Mesh());

    std::stringstream stream;
    stream << "_" << static_cast<void*>(meshes[0]);
    std::string meshId = stream.str();

    _meshesMap[meshId] = std::vector<std::unique_ptr<Mesh>>();
    for (Mesh* mesh : meshes) {
        _meshesMap[meshId].push_back(std::unique_ptr<Mesh>(mesh));
    }
    if (id) {
        *id = meshId;
    }

    return 0;
}

int MeshManager::createOrReplace(std::string id, size_t nbMeshes, std::vector<Mesh*>& meshes)
{
    if (!isIdValid(id) || nbMeshes == 0) {
        return -1;
    }

    _meshesMap.erase(id);

    meshes = std::vector<Mesh*>(nbMeshes, new Mesh());

    _meshesMap[id] = std::vector<std::unique_ptr<Mesh>>();
    for (Mesh* mesh : meshes) {
        _meshesMap[id].push_back(std::unique_ptr<Mesh>(mesh));
    }

    return 0;
}

int MeshManager::get(std::string id, std::vector<Mesh*>& meshes)
{
    auto found = _meshesMap.find(id);
    if (found != _meshesMap.end()) {
        for (const std::unique_ptr<Mesh>& mesh : _meshesMap[id]) {
            meshes.push_back(mesh.get());
        }
        return 0;
    }
    return -1;
}

int MeshManager::getFromFileName(std::string fileName, std::vector<Mesh*>& meshes)
{
    auto idFound = _fileNameToId.find(fileName);
    if (idFound != _fileNameToId.end()) {
        return get(idFound->second, meshes);
    }
    return get("_" + fileName, meshes);
}

namespace {
    bool isIdValid(std::string id)
    {
        return id.size() && id[0] != '_';
    }
}
