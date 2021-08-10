#pragma once

#include <embree3/rtcore.h>

#include <vector>
#include <memory>

struct Vertex;

class Embree
{
public:
	~Embree();
	static Embree* getInstance();
	bool addGeometry(const std::vector<std::shared_ptr<Vertex>>, const std::vector<unsigned int> &indices);

private:
	Embree();
	static Embree *_instance;

private:
	RTCDevice _device;
	RTCScene _scene;

};

