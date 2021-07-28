#include "Application.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

Application::Application() :
    _camera(glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0, 1, 0), (40.f * float(M_PI)) / 180.f, static_cast<float>(DEFAULT_WINDOW_WIDTH) / DEFAULT_WINDOW_HEIGHT),
    _inputManager(*this)
{
}

Application::~Application()
{
}

int Application::init()
{
    if (_initWindow())
        return -1;

    _inputManager.init(_window);

    if (_vulkan.init(_window))
        return -1;

    return 0;
}

int Application::_initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "AurellRayTracer", nullptr, nullptr);

    if (_window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(_window);

    return 0;
}
