#include "Embree.h"

Embree* Embree::_instance = nullptr;

Embree::Embree() :
	_device(rtcNewDevice(nullptr)), _scene(rtcNewScene(_device))
{
}

Embree::~Embree()
{
}

Embree* Embree::getInstance()
{
	if (_instance)
		return _instance;
	return _instance = new Embree();
}
