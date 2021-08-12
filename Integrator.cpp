#include "Integrator.h"

Integrator::Integrator(size_t imageWidth, size_t imageHeight, size_t nbChannels) :
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

size_t Integrator::getImageWidth() const
{
	return _imageWidth;
}

size_t Integrator::getImageHeight() const
{
	return _imageHeight;
}

size_t Integrator::getNbChannels() const
{
	return _nbChannels;
}
