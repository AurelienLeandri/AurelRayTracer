#pragma once

#include <vector>

class Camera;
class Scene;

class Integrator
{
public:
    Integrator(size_t imageWidth, size_t imageHeight, size_t nbChannels);

public:
    virtual int init() = 0;
    virtual bool iterate() = 0;
    virtual int cleanup() = 0;

    void setCamera(const Camera* camera);
    void setScene(const Scene* scene);

    const std::vector<unsigned char>& getImageBuffer() const;
    size_t getImageWidth() const;
    size_t getImageHeight() const;
    size_t getNbChannels() const;

protected:
    const Camera* _camera = nullptr;
    const Scene* _scene = nullptr;
    std::vector<unsigned char> _imageBuffer;
    size_t _imageWidth = 0;
    size_t _imageHeight = 0;
    size_t _nbChannels = 0;
};

