#pragma once
#include "Texture.h"

#include <memory>
#include <string>

class ImageTexture :
    public Texture
{
public:
    ImageTexture(int width, int height, int nbChannels);
    ImageTexture(std::string path);
    // Must be copied using Copy function
    ImageTexture(const ImageTexture &other) = delete;
    ImageTexture &operator=(const ImageTexture &other) = delete;
    // Move is okay
    ImageTexture(const ImageTexture &&other);
    ImageTexture &operator=(const ImageTexture &&other);
    virtual ~ImageTexture();

    virtual glm::vec3 color(float u, float v, const glm::vec3& p) const;
    void setFactor(const glm::vec3 &factor);

private:
    // Owns
    unsigned char *_data = nullptr;

    int _width = 0;
    int _height = 0;
    int _nbChannels = 0;
    std::string _path;
    glm::vec3 _factor = glm::vec3(1, 1, 1);
};

