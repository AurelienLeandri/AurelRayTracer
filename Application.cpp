#include "Application.h"

#include <iostream>

namespace {
    const char* vertexShaderSource = "#version 460 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aUV;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);\n"
        "   TexCoord = vec2(aUV.x, aUV.y);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 460 core\n"
        "in  vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(ourTexture, vec2(1.0 - TexCoord.x, 1.0 - TexCoord.y));\n"  // Sensor rotation
        "}\n\0";

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> extensions = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
}

Application::~Application()
{
    cleanup();
}

bool Application::refreshWindow(const float* image_buffer)
{
    bool window_open = _processInput();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, image_buffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(_window);
    glfwPollEvents();
    return window_open;
}

int Application::_openglInit() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow(WIDTH * _zoomFactor, HEIGHT * _zoomFactor, "AurellRayTracer", NULL, NULL);
    if (_window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(_window);

    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float quad_vbo[] = {
        //   x   y   u  v
            -1, -1,  0, 0,  // bottom left
            -1,  1,  0, 1,  // bottom right
                1, -1,  1, 0,  // top left
                1,  1,  1, 1,  // top right
    };

    unsigned int quad_ebo[] = {
        3, 1, 2, 1, 0, 2
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vbo), quad_vbo, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_ebo), quad_ebo, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    //glGenerateMipmap(GL_TEXTURE_2D);

    glViewport(0, 0, WIDTH * _zoomFactor, HEIGHT * _zoomFactor);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    return 0;
}


int Application::_vulkanInit() {
    createInstance();
    return 0;
}

void Application::run()
{
    if (!initWindow()) {
        _vulkanInit();
        setupDebugMessenger();
        pickPhysicalDevice();
        createLogicalDevice();
        mainLoop();
        cleanup();
    }
}

void Application::mainLoop()
{
}

void Application::cleanup()
{
    vkDestroyDevice(device, nullptr);

    if (debugMessenger) {
        if (auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")) {
            vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            debugMessenger = nullptr;
        }
        else
            throw std::runtime_error("Failed to load extension function vkDestroyDebugUtilsMessengerEXT");
    }

    if (instance) {
        vkDestroyInstance(instance, nullptr);
        instance = nullptr;
    }
    if (_window) {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }
    glfwTerminate();
}

int Application::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(WIDTH * _zoomFactor, HEIGHT * _zoomFactor, "AurellRayTracer", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    if (_window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(_window);

    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);

    return 0;
}

namespace {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool hasMandatoryFamilies() {
            return graphicsFamily.has_value();
        }
    };

    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        }

        return VK_FALSE;
    }

    void getRequiredExtensions(std::vector<const char*>& requiredExtensions) {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        requiredExtensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            for (const char* extensionName : extensions) {
                requiredExtensions.push_back(extensionName);
            }
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    QueueFamilyIndices findRequiredQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (int i = 0; i < queueFamilyCount; ++i) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
        }

        return indices;
    }

    int ratePhysicalDevice(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        if (!deviceFeatures.geometryShader) {
            // Application can't function without geometry shaders
            return 0;
        }

        // Check for the queue families available on the device
        QueueFamilyIndices indices = findRequiredQueueFamilies(device);
        if (!indices.hasMandatoryFamilies()) {
            return 0;
        }

        int score = 0;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        return score;
    }
}


void Application::createInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "AurellRayTracer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Get the extension from GLFW so that Vulkan can interface with the window system
    // TODO: store requiredExtensions as a "options" parameter or as member variable
    std::vector<const char*> requiredExtensions;
    getRequiredExtensions(requiredExtensions);
    if (checkRequiredExtensionsSupport(requiredExtensions)) {
        createInfo.enabledExtensionCount = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    }

    // Validation layers
    if (enableValidationLayers && checkValidationLayerSupport(validationLayers)) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        // Create a debug messenger specificaly for instance creation and destruction.
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance)) {
        throw std::runtime_error("Failed to create instance!");
    }

}

void Application::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    populateDebugMessengerCreateInfo(createInfo);

    // vkCreateDebugUtilsMessengerEXT is an extension function so we have to look up its address ourselves using vkGetInstanceProcAddr
    if (auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")) {
        if (vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger))
            throw std::runtime_error("Failed to create required VkDebugUtilsMessengerEXT!");
    }
    else
        throw std::runtime_error("Failed to load extension function vkCreateDebugUtilsMessengerEXT");

}

void Application::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (!deviceCount) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    const VkPhysicalDevice* candidateDevice = nullptr;
    int maxScore = 0;
    for (const VkPhysicalDevice& device : devices) {
        int score = ratePhysicalDevice(device);
        if (score && score > maxScore) {
            candidateDevice = &device;
            maxScore = score;
        }
    }

    if (!candidateDevice) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    physicalDevice = *candidateDevice;
}

void Application::createLogicalDevice() {
    QueueFamilyIndices indices = findRequiredQueueFamilies(physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;
    // Device-specific validation layers are not used anymore. We set them regardless for backward compatibility
    createInfo.enabledLayerCount = 0;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}

bool Application::checkRequiredExtensionsSupport(const std::vector<const char*> &requiredExtensions) {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    // TODO: remove
    std::cout << "Available extensions:\n";
    for (const VkExtensionProperties& availableExtension : availableExtensions) {
        std::cout << '\t' << availableExtension.extensionName << '\n';
    }

    // Check for GLFW extensions support
    const char* extensionNotFound = nullptr;
    for (int i = 0; i < requiredExtensions.size(); ++i) {
        bool found = false;
        for (const VkExtensionProperties& availableExtension : availableExtensions) {
            if (!strcmp(availableExtension.extensionName, requiredExtensions[i])) {
                found = true;
                break;
            }
        }
        if (!found) {
            extensionNotFound = requiredExtensions[i];
            break;
        }
    }
    if (extensionNotFound) {
        throw std::runtime_error(std::string("Required GLFW extension \"") + extensionNotFound + "\" not found in Vulkan implementation");
    }

    return !extensionNotFound;
}

bool Application::checkValidationLayerSupport(const std::vector<const char*>& requiredLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // TODO: remove
    std::cout << "Available validation layers:\n";
    for (const VkLayerProperties& layer : availableLayers) {
        std::cout << '\t' << layer.layerName << '\n';
    }

    // Check for required validation layers support
    const char* layerNotFound = nullptr;
    for (int i = 0; i < requiredLayers.size(); ++i) {
        bool found = false;
        for (const VkLayerProperties& layer : availableLayers) {
            if (!strcmp(layer.layerName, requiredLayers[i])) {
                found = true;
                break;
            }
        }
        if (!found) {
            layerNotFound = requiredLayers[i];
            break;
        }
    }
    if (layerNotFound) {
        throw std::runtime_error(std::string("Required validation layer \"") + layerNotFound + "\" not found");
    }

    return !layerNotFound;
}

int Application::init()
{
    //return _openglInit();
    try {
        run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

void Application::idle() {
    while (_window) {
        _processInput();
        glfwPollEvents();
    }
}

bool Application::_processInput()
{
    return !(glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(_window));
}
