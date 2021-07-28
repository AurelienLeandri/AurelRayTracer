#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

class Application;

class InputManager
{
public:
	InputManager(Application& application);

public:
	void init(GLFWwindow* window);

private:
	static void _framebufferResizeCallback(GLFWwindow* window, int width, int height);
	static void _mouseCallback(GLFWwindow* window, double xpos, double ypos);

private:
	Application& _application;
	GLFWwindow* _window = nullptr;
};

