#include "Texture.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "HitRecord.h"

#include <algorithm>

ImageTexture::ImageTexture(std::string path)
{
    _data = std::make_shared<_ImageTextureData>(path);
}

glm::vec3 ImageTexture::getColor(const HitRecord& hit_record) const
{
    if (_data)
        return _data->getColor(hit_record);
    return glm::vec3(0);
}

ImageTexture::_ImageTextureData::_ImageTextureData(std::string path)
{
    stbi_set_flip_vertically_on_load(false);
    _data = stbi_load(path.c_str(), &_width, &_height, &_nbChannels, 0);
}

ImageTexture::_ImageTextureData::~_ImageTextureData()
{
    if (!_data)
        return;
    if (_path.size())
        stbi_image_free(_data);
    else
        delete[] _data;
}

glm::vec3 ImageTexture::_ImageTextureData::getColor(const HitRecord& hit_record) const
{
    if (_data) {
        float u = fmod(hit_record.u, 1.0f);
        float v = fmod(hit_record.v, 1.0f);
        if (v > 1 || v < 0 || u < 0 || u > 1) {
            int a = 0;
            a = a;
        }
        int i = (int)std::max(0.f, std::min(hit_record.u * _width, (float)(_width - 1)));
        int j = (int)std::max(0.f, std::min((1.f - hit_record.v) * _height, (float)(_height - 1)));
        int index = (_nbChannels * _width) * j + (i * _nbChannels);
        static const float INV_255 = 1.f / 255;
        return glm::vec3(_data[index] * INV_255, _data[index + 1] * INV_255, _data[index + 2] * INV_255);
    }
    return glm::vec3(0);
}



std::shared_ptr<ConstantTexture> ConstantTexture::white = std::make_shared<ConstantTexture>(glm::vec3(1));
std::shared_ptr<ConstantTexture> ConstantTexture::black = std::make_shared<ConstantTexture>(glm::vec3(0));
std::shared_ptr<ConstantTexture> ConstantTexture::blue = std::make_shared<ConstantTexture>(glm::vec3(0, 0, 1));

ConstantTexture::ConstantTexture()
{
}

ConstantTexture::ConstantTexture(const glm::vec3& color) : color(color)
{
}

glm::vec3 ConstantTexture::getColor(const HitRecord& hit_record) const
{
    return color;
}
