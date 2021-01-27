#include "ImageTexture.h"

#include <glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>

ImageTexture::ImageTexture(int width, int height, int nbChannels) : Texture(), _width(width), _height(height), _nbChannels(nbChannels)
{
    _data = new unsigned char[_width * _height * _nbChannels]();
}

ImageTexture::ImageTexture(std::string path) : _path(path)
{
    _data = stbi_load(path.c_str(), &_width, &_height, &_nbChannels, 0);
}


ImageTexture::ImageTexture(const ImageTexture && other) : _data(other._data), _width(other._width), _height(other._height), _nbChannels(other._nbChannels), _path(other._path)
{
}

ImageTexture & ImageTexture::operator=(const ImageTexture && other)
{
    _data = other._data;
    _width = other._width;
    _height = other._height;
    _nbChannels = other._nbChannels;
    _path = other._path;
    return *this;
}

ImageTexture::~ImageTexture()
{
    if (!_data)
        return;
    if (_path.size())
        stbi_image_free(_data);
    else
        delete[] _data;
}

glm::vec3 ImageTexture::color(float u, float v, const glm::vec3 & p) const
{
    int i = (int) std::max(0.f, std::min(u * _width, (float)(_width - 1)));
    int j = (int) std::max(0.f, std::min((1 - v) * _height, (float)(_height - 1)));
    int index = (_nbChannels * _width) * j + (i * _nbChannels);
    float inv_255 = 1.f / 255;
    return glm::vec3(_data[index] * inv_255, _data[index + 1] * inv_255, _data[index + 2] * inv_255);
}
