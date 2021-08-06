#include "Integrator.h"

Integrator::Integrator(unsigned int imageWidth, unsigned int imageHeight, unsigned int nbChannels) :
	_imageWidth(imageWidth), _imageHeight(imageHeight), _nbChannels(nbChannels)
{
	_imageBuffer.resize(static_cast<size_t>(_imageWidth) * _imageHeight * _nbChannels, 255);
}

void Integrator::setCamera(const Camera* camera)
{
	_camera = camera;
}

void Integrator::setScene(const Scene* scene)
{
	_scene = scene;
}

const std::vector<unsigned char>& Integrator::getImageBuffer() const
{
	return _imageBuffer;
}

unsigned int Integrator::getImageWidth() const
{
	return _imageWidth;
}

unsigned int Integrator::getImageHeight() const
{
	return _imageHeight;
}

unsigned int Integrator::getNbChannels() const
{
	return _nbChannels;
}
