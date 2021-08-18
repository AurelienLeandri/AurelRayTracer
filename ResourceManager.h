#pragma once

#include <memory>
#include <unordered_map>

#include "Texture.h"
#include "Mesh.h"

/* static */ class ImageTextureManager
{
public:
	static int loadFromFile(std::string fileName, ImageTexture*& texture, std::string* id = nullptr);
	static int loadFromFile(std::string id, std::string fileName, ImageTexture*& texture);

	static int add(std::string id, ImageTexture* texture);
	static int add(ImageTexture* texture, std::string* id = nullptr);

	static int addOrReplace(std::string id, ImageTexture* texture);

	static int create(std::string id, size_t width, size_t height, size_t nbChannels, ImageTexture*& texture);
	static int create(size_t width, size_t height, size_t nbChannels, ImageTexture*& texture, std::string* id = nullptr);

	static int createOrReplace(std::string id, size_t width, size_t height, size_t nbChannels, ImageTexture*& texture);

	static int get(std::string id, ImageTexture*& texture);

	static int getFromFileName(std::string fileName, ImageTexture*& texture);

private:
	static std::unordered_map<std::string, std::unique_ptr<ImageTexture>> _texturesMap;
	static std::unordered_map<std::string, std::string> _fileNameToId;
};

/* static */ class MeshManager
{
public:
	static int loadFromFile(std::string fileName, std::vector<Mesh*>& meshes, std::string* id = nullptr);
	static int loadFromFile(std::string id, std::string fileName, std::vector<Mesh*>& meshes);

	static int add(std::string id, const std::vector<Mesh*>& meshes);
	static int add(const std::vector<Mesh*>& meshes, std::string* id = nullptr);

	static int addOrReplace(std::string id, const std::vector<Mesh*>& meshes);

	static int create(std::string id, size_t nbMeshes, std::vector<Mesh*>& meshes);
	static int create(size_t nbMeshes, std::vector<Mesh*>& meshes, std::string* id = nullptr);

	static int createOrReplace(std::string id, size_t nbMeshes, std::vector<Mesh*>& meshes);

	static int get(std::string id, std::vector<Mesh*>& meshes);

	static int getFromFileName(std::string fileName, std::vector<Mesh*>& meshes);

private:
	static std::unordered_map<std::string, std::vector<std::unique_ptr<Mesh>>> _meshesMap;
	static std::unordered_map<std::string, std::string> _fileNameToId;
};

