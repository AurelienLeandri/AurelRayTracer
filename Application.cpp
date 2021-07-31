#include "Application.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

#include "NavigationRenderer.h"
#include "Scene.h"
#include "ModelLoader.h"

Application::Application() :
    _camera(glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0, 1, 0), (40.f * float(M_PI)) / 180.f, static_cast<float>(DEFAULT_WINDOW_WIDTH) / DEFAULT_WINDOW_HEIGHT),
    _inputManager(*this)
{
}

Application::~Application()
{
    std::string failed;
    for (std::pair<const std::string, std::unique_ptr<Renderer>>& pair : _renderers) {
        std::unique_ptr<Renderer>& renderer = pair.second;
        if (renderer->cleanup()) {
            failed = pair.first;
            break;
        }
    }

    if (failed.size()) {
        std::cerr << "Failed to cleanup renderer " << failed << std::endl;
    }

    _vulkan.cleanup();

    glfwDestroyWindow(_window);
    _window = nullptr;

    glfwTerminate();
}

int Application::init()
{
    if (_initWindow()) {
        std::cerr << "Failed to initialize window context" << std::endl;
        return -1;
    }

    _inputManager.init(_window, &_camera);

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
        renderer->setCamera(_camera);
        renderer->setScene(_scene);
    }

    for (std::pair<const std::string, std::unique_ptr<Renderer>>& pair : _renderers) {
        std::unique_ptr<Renderer>& renderer = pair.second;
        if (renderer->init()) {
            failed = pair.first;
            break;
        }
    }

    return failed.size() ? -1 : 0;
}

void Application::_cleanUp()
{
}

void Application::loadScene(const std::string& fileName)
{
    // TODO: load scene and remove this temp code
    auto s = SceneFactory::createScene();
    TransformParameters t;
    const std::string MODEL_PATH = "viking_room/viking_room.obj";
    std::string model_path = MODEL_PATH;
    if (!ModelLoader::loadModel(model_path, *s, Transform(t))) {
        std::cerr << "Could not load model " << model_path << std::endl;
        return;
    }

    _scene = s;

}

void Application::mainLoop()
{
    while (_inputManager.processInput()) {
        _renderers["NavigationRenderer"]->iterate();
    }
}
