#pragma once

#include <vector>
#include <memory>
#include <string>

class HitableList;
class Texture;

class ModelLoader
{
public:
	static std::shared_ptr<HitableList> loadModel(std::string path);
};

