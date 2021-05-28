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
	virtual glm::vec3 getColor(const HitRecord &hit_record) const = 0;
};

class ConstantTexture :
    public Texture
{
public:
    ConstantTexture();
    ConstantTexture(const glm::vec3& color);

public:
    virtual glm::vec3 getColor(const HitRecord& hit_record) const;

public:
    glm::vec3 color = glm::vec3(1);
    static std::shared_ptr<ConstantTexture> white;
    static std::shared_ptr<ConstantTexture> black;
    static std::shared_ptr<ConstantTexture> blue;
};

class ImageTexture :
    public Texture
{
public:
    explicit ImageTexture(std::string path);

public:
    virtual glm::vec3 getColor(const HitRecord& hit_record) const;
    int getWidth() const;
    int getHeight() const;
    const unsigned char* getData() const;
    int getNbChannels() const;

private:
    class _ImageTextureData :
        public Texture
    {
    public:
        _ImageTextureData(std::string path);
        ~_ImageTextureData();
        _ImageTextureData(const ImageTexture& other) = delete;
        _ImageTextureData& operator=(const ImageTexture& other) = delete;
        _ImageTextureData(const ImageTexture&& other) = delete;
        _ImageTextureData& operator=(const ImageTexture&& other) = delete;

        virtual glm::vec3 getColor(const HitRecord& hit_record) const;

    public:
        // Owns
        unsigned char* _data = nullptr;

        int _width = 0;
        int _height = 0;
        int _nbChannels = 0;
        std::string _path;
    };

private:
    std::shared_ptr<_ImageTextureData> _data = nullptr;
};

