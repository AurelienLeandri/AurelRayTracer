#pragma once

#include <memory>
#include <unordered_map>

#include "Camera.h"
#include "InputManager.h"
#include "VulkanInstance.h"

class Scene;
class Renderer;

class Application
{
public:
	Application();
	~Application();

public:
	int init();

private:
	int _initWindow();
	void _initRenderers();
	void _loadScene(const char* fileName);
	void _mainLoop();
	void _cleanUp();

private:
	static const unsigned int DEFAULT_WINDOW_WIDTH = 1200;
	static const unsigned int DEFAULT_WINDOW_HEIGHT = 1200;

private:
	//std::unique_ptr<Scene> _scene = nullptr;
	//std::unordered_map<const char*, std::unique_ptr<Renderer>> _renderers;
	InputManager _inputManager;
	VulkanInstance _vulkan;
	Camera _camera;
	GLFWwindow* _window = nullptr;

	friend class InputManager;
};

