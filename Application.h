#pragma once

#include <memory>
#include <unordered_map>

#include "Camera.h"
#include "InputManager.h"
#include "VulkanInstance.h"
#include "Renderer.h"

class Scene;
class Renderer;

class Application
{
public:
	Application();
	~Application();

public:
	int init();
	void loadScene(const std::string& fileName);
	void mainLoop();

private:
	int _initWindow();
	int _initRenderers(std::string& failed);
	void _cleanUp();

private:
	static const unsigned int DEFAULT_WINDOW_WIDTH = 1200;
	static const unsigned int DEFAULT_WINDOW_HEIGHT = 1200;

private:
	//std::unique_ptr<Scene> _scene = nullptr;
	std::unordered_map<std::string, std::unique_ptr<Renderer>> _renderers;
	InputManager _inputManager;
	VulkanInstance _vulkan;
	Camera _camera;
	GLFWwindow* _window = nullptr;
	Scene* _scene = nullptr;

	friend class InputManager;
};

