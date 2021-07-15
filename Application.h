#pragma once

#include <glad/glad.h>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include <vector>
#include <optional>

class Application
{
public:
    Application() = default;
    ~Application();

public:
    bool refreshWindow(const float* image_buffer);
    int init();
    void idle();

public:
    // Window information
    static const unsigned int WIDTH = 600;
    static const unsigned int HEIGHT = 600;
    static const unsigned int NB_CHANNELS = 3;

private:
    bool _processInput();
    int _openglInit();
    int _vulkanInit();

// TODO: refactor this once it runs
public:
    void run();

private:
    void mainLoop();
    void cleanup();
    int initWindow();
    void createInstance();
    bool checkValidationLayerSupport(const std::vector<const char*>& requiredLayers);
    bool checkRequiredExtensionsSupport(const std::vector<const char*>& requiredExtensions);
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
// END TODO

private:
    // Application window
    GLFWwindow* _window = nullptr;
    unsigned int _zoomFactor = 2;


// TODO: refactor this once it runs
private:
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
// END TODO
};
