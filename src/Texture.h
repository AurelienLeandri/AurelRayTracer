#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>

struct HitRecord;

class Texture
{
public:
	Texture() = default;;
	virtual ~Texture() = default;;

public:
    virtual glm::vec3 getColor(const HitRecord& hit_record) const = 0;
    virtual glm::vec3 getColor(float u, float v) const = 0;
};

class ConstantTexture :
    public Texture
{
public:
    ConstantTexture();
    ConstantTexture(const glm::vec3& color);

public:
    virtual glm::vec3 getColor(const HitRecord& hit_record) const;
    virtual glm::vec3 getColor(float u, float v) const;

public:
    glm::vec3 color = glm::vec3(1);
    static std::shared_ptr<ConstantTexture> white;
    static std::shared_ptr<ConstantTexture> black;
    static std::shared_ptr<ConstantTexture> blue;
};

enum class ImageTextureDataType {
    UNSIGNED_CHAR,
    FLOAT
};

enum class ImageTextureLayoutType {
    RGB,
    LUMINANCE
};

class ImageTexture :
    public Texture
{
public:
    ImageTexture(std::string path, ImageTextureDataType type = ImageTextureDataType::FLOAT, ImageTextureLayoutType layoutType = ImageTextureLayoutType::RGB);

public:
    virtual glm::vec3 getColor(const HitRecord& hit_record) const;
    virtual glm::vec3 getColor(float u, float v) const;
    int getWidth() const;
    int getHeight() const;
    const void* getData() const;
    void* getData();
    int getNbChannels() const;

private:
    class _ImageTextureData :
        public Texture
    {
    public:
        _ImageTextureData(std::string path, ImageTextureDataType type, ImageTextureLayoutType layoutType);
        ~_ImageTextureData();
        _ImageTextureData(const ImageTexture& other) = delete;
        _ImageTextureData& operator=(const ImageTexture& other) = delete;
        _ImageTextureData(const ImageTexture&& other) = delete;
        _ImageTextureData& operator=(const ImageTexture&& other) = delete;

        virtual glm::vec3 getColor(const HitRecord& hit_record) const;
        virtual glm::vec3 getColor(float u, float v) const;

    private:
        void freeTexels();

    public:
        // Owns
        void* texels = nullptr;

        int width = 0;
        int height = 0;
        int nbChannels = 0;
        std::string path;
        ImageTextureDataType dataType;
        ImageTextureLayoutType layoutType;
    };

private:
    std::shared_ptr<_ImageTextureData> _data = nullptr;
};

