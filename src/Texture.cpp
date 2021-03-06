#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "HitRecord.h"

#include <algorithm>

ImageTexture::ImageTexture(std::string path, ImageTextureDataType type, ImageTextureLayoutType layoutType)
{
    _data = std::make_shared<_ImageTextureData>(path, type, layoutType);
}

glm::vec3 ImageTexture::getColor(const HitRecord& hit_record) const
{
    if (_data)
        return _data->getColor(hit_record);
    return glm::vec3(0);
}

glm::vec3 ImageTexture::getColor(float u, float v) const
{
    if (_data)
        return _data->getColor(u, v);
    return glm::vec3(0);
}

int ImageTexture::getWidth() const
{
    return _data->width;
}

int ImageTexture::getHeight() const
{
    return _data->height;
}

const void* ImageTexture::getData() const
{
    return _data->texels;
}

void* ImageTexture::getData()
{
    return _data->texels;
}

int ImageTexture::getNbChannels() const
{
    return _data->nbChannels;
}

namespace {
    template <typename T>
    void rgbToLuminance(const T* src, T* dest, int width, int height, int srcNbCHannels) {
        for (int i = 0; i < width * height * srcNbCHannels; i += srcNbCHannels) {
            dest[i / 3] = (T)(src[i] * 0.3f + src[i + 1] * 0.59f + src[i + 2] * 0.11f);
        }
    }

    void gammaToLinear(float* texels, int width, int height, int nbChannels) {
        for (int i = 0; i < width * height * nbChannels; i += nbChannels) {
            for (int k = 0; k < glm::min(nbChannels, 3); ++k) {
                //texels[i + k] = glm::pow(texels[i + k], 2.2f);
            }
        }
    }
}

ImageTexture::_ImageTextureData::_ImageTextureData(std::string path, ImageTextureDataType dataType, ImageTextureLayoutType layoutType)
    : dataType(dataType), layoutType(layoutType)
{
    stbi_set_flip_vertically_on_load(false);
    if (dataType == ImageTextureDataType::UNSIGNED_CHAR) {
        texels = stbi_load(path.c_str(), &width, &height, &nbChannels, 0);
    }
    else if (dataType == ImageTextureDataType::FLOAT) {
        texels = stbi_loadf(path.c_str(), &width, &height, &nbChannels, 0);
        gammaToLinear((float*)texels, width, height, nbChannels);
    }
    if (layoutType == ImageTextureLayoutType::LUMINANCE) {
        if (nbChannels == 3) {  // Convert the first three channels of each texel to luminance
            if (dataType == ImageTextureDataType::FLOAT) {
                float *luminanceTexels = new float[(size_t)width * height];
                rgbToLuminance<float>((float *)texels, luminanceTexels, width, height, nbChannels);
                freeTexels();
                texels = luminanceTexels;
            }
            else if (dataType == ImageTextureDataType::UNSIGNED_CHAR) {
                unsigned char* luminanceTexels = new unsigned char[(size_t)width * height];
                rgbToLuminance<unsigned char>((unsigned char*)texels, luminanceTexels, width, height, nbChannels);
                freeTexels();
                texels = luminanceTexels;
            }
            path = "";  // Texels were generated by us, and we got rid of the previous texture. This is done to avoid calling stbi_free.
            nbChannels = 1;
        }
    }
}

ImageTexture::_ImageTextureData::~_ImageTextureData()
{
    freeTexels();
}

glm::vec3 ImageTexture::_ImageTextureData::getColor(const HitRecord& hit_record) const
{
    return getColor(hit_record.u, hit_record.v);
}

glm::vec3 ImageTexture::_ImageTextureData::getColor(float u, float v) const
{
    if (texels) {
        int i = (int)std::max(0.f, std::min(u * width, (float)(width - 1)));
        int j = (int)std::max(0.f, std::min((1.f - v) * height, (float)(height - 1)));
        int index = (nbChannels * width) * j + (i * nbChannels);
        if (dataType == ImageTextureDataType::UNSIGNED_CHAR) {
            unsigned char* charData = (unsigned char*)texels;
            return glm::vec3(charData[index], charData[index + 1], charData[index + 2]);
        }
        else if (dataType == ImageTextureDataType::FLOAT) {
            float* floatData = (float*)texels;
            return glm::vec3(floatData[index], floatData[index + 1], floatData[index + 2]);
        }
    }
    return glm::vec3(0);
}

void ImageTexture::_ImageTextureData::freeTexels()
{
    if (!texels)
        return;
    if (path.size())
        stbi_image_free(texels);
    else
        delete[] texels;
    texels = nullptr;
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

glm::vec3 ConstantTexture::getColor(float u, float v) const
{
    return color;
}
