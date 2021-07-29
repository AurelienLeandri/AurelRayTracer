#include "InputManager.h"

#include "Application.h"

bool InputManager::framebufferResized = false;

InputManager::InputManager(Application& application) :
    _application(application)
{
}

void InputManager::init(GLFWwindow* window)
{
	_window = window;

    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(_window, &_application);
    glfwSetFramebufferSizeCallback(_window, _framebufferResizeCallback);
    glfwSetCursorPosCallback(_window, _mouseCallback);
}

void InputManager::_framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    Application* app = (Application*)(glfwGetWindowUserPointer(window));
    framebufferResized = true;
}

void InputManager::_mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstTime = true;
    static float lastX = 0, lastY = 0;
    if (firstTime)
    {
        lastX = xpos;
        lastY = ypos;
        firstTime = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    Application* app = (Application*)(glfwGetWindowUserPointer(window));
    //app->_inputManager.processMouseMovement(xoffset, yoffset);
}
