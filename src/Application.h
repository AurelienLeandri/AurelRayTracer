#pragma once

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

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
    static const int WIDTH = 600;
    static const int HEIGHT = 600;
    static const int NB_CHANNELS = 3;

private:
    bool _processInput();

private:
    // Application window
    GLFWwindow* _window = nullptr;
    int _zoomFactor = 2;
};
