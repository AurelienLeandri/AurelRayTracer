#pragma once

#include <vector>

class Camera;
class Scene;

class Integrator
{
public:
    Integrator(unsigned int imageWidth, unsigned int imageHeight, unsigned int nbChannels);

public:
    virtual int init() = 0;
    virtual bool iterate() = 0;
    virtual int cleanup() = 0;

    void setCamera(const Camera* camera);
    void setScene(const Scene* scene);

    const std::vector<unsigned char>& getImageBuffer() const;
    unsigned int getImageWidth() const;
    unsigned int getImageHeight() const;
    unsigned int getNbChannels() const;

protected:
    const Camera* _camera = nullptr;
    const Scene* _scene = nullptr;
    std::vector<unsigned char> _imageBuffer;
    unsigned int _imageWidth = 0;
    unsigned int _imageHeight = 0;
    unsigned int _nbChannels = 0;
};

