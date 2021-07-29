#include "Application.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

#include "NavigationRenderer.h"

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
    if (_initWindow()) {
        std::cerr << "Failed to initialize window context" << std::endl;
        return -1;
    }

    _inputManager.init(_window);

    if (_vulkan.init(_window)) {
        std::cerr << "Failed to initialize Vulkan instance" << std::endl;
        return -1;
    }

    std::string failed;
    if (_initRenderers(failed)) {
        std::cerr << "Failed to initialize renderer " << failed << std::endl;
        return -1;
    }

    return 0;
}

int Application::_initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "AurellEngine", nullptr, nullptr);

    if (_window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(_window);

    return 0;
}

int Application::_initRenderers(std::string& failed)
{
    _renderers["NavigationRenderer"] = std::make_unique<NavigationRenderer>(*_window, _vulkan);
    
    for (std::pair<const std::string, std::unique_ptr<Renderer>>& pair : _renderers) {
        std::unique_ptr<Renderer>& renderer = pair.second;
        if (renderer->init()) {
            failed = pair.first;
            break;
        }
    }

    return failed.size() ? -1 : 0;
}
