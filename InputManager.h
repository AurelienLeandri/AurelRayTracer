#pragma once

#include <glad/glad.h>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

class Application;

class InputManager
{
public:
	InputManager(Application& application);

public:
	void init(GLFWwindow* window);

public:
	// TODO: refactor this
	static bool framebufferResized;

private:
	static void _framebufferResizeCallback(GLFWwindow* window, int width, int height);
	static void _mouseCallback(GLFWwindow* window, double xpos, double ypos);

private:
	Application& _application;
	GLFWwindow* _window = nullptr;
};

