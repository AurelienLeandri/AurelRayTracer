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
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentationFamily;

        bool hasMandatoryFamilies() {
            return graphicsFamily.has_value() && presentationFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

private:
    void mainLoop();
    void cleanup();
    int initWindow();
    void createInstance();
    bool checkValidationLayerSupport(const std::vector<const char*>& requiredLayers);
    bool checkInstanceRequiredExtensionsSupport(const std::vector<const char*>& requiredExtensions);
    bool checkDeviceRequiredExtensionSupport(VkPhysicalDevice device);
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSurface();
    QueueFamilyIndices findRequiredQueueFamilies(VkPhysicalDevice device);
    int ratePhysicalDevice(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void createSwapChain();
    void createImageViews();
    void createGraphicsPipeline();
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createRenderPass();
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
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentationQueue = VK_NULL_HANDLE;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
// END TODO
};
