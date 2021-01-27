// AurellRayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <ctime>

#include <omp.h>
#include <limits>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLFW_INCLUDE_NONE
#include "glfw3.h"

#include "glad/glad.h"

#include "Camera.h"
#include "Ray.h"
#include "Utils.h"
#include "Sphere.h"
#include "HitableList.h"
#include "ImageTexture.h"

glm::vec3 get_color(const Ray& r, const Hitable& world, size_t depth = 10000);

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

class Application
{
public:
    Application() = default;
    ~Application();

public:
    int refreshWindow(const float* image_buffer);
    int init();

public:
    // Window information
    static const unsigned int WIDTH = 800;
    static const unsigned int HEIGHT = 600;
    static const unsigned int NB_CHANNELS = 3;

private:
    // Application window
    GLFWwindow* window = nullptr;

    // OpenGL
};

Application::~Application()
{
    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();
}

int Application::refreshWindow(const float* image_buffer)
{
    if (!image_buffer) return -1;

    processInput(window);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, image_buffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
    return 0;
}

int Application::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "AurellRayTracer", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    return 0;
}

class RayTracer {
public:
    RayTracer() = default;
    ~RayTracer();

public:
    int init();
    bool iterate();
    const float* getImageBuffer() const;
    void setWorld(std::shared_ptr<Hitable> world);
    void setCamera(std::shared_ptr<Camera> camera);

private:
    float* image_buffer = nullptr;

    const unsigned int WIDTH = Application::WIDTH;
    const unsigned int HEIGHT = Application::HEIGHT;
    const unsigned int NB_CHANNELS = Application::NB_CHANNELS;

    const unsigned int NB_SAMPLES = 10;
    const float INV_NB_SAMPLES = 1.f / NB_SAMPLES;
    const unsigned int NB_THREADS = omp_get_max_threads();
    const size_t PORTION_SIZE = (WIDTH * HEIGHT) / NB_THREADS;

    std::shared_ptr<Camera> camera;
    std::shared_ptr<Hitable> world;

    unsigned int sample = 1;
};

RayTracer::~RayTracer()
{
    if (image_buffer) {
        delete[] image_buffer;
        image_buffer = nullptr;
    }
}

int RayTracer::init()
{
    image_buffer = new float[WIDTH * HEIGHT * NB_CHANNELS];
    for (int i = 0; i < WIDTH * HEIGHT * NB_CHANNELS; ++i)
        image_buffer[i] = 1.f;

    return 0;
}

bool RayTracer::iterate() {
    std::cout << "\tSample " << sample;

    std::clock_t start(std::clock());

    float buffer_factor = ((sample - 1.f) / sample);
    float color_factor = (1.f / sample);

#pragma omp parallel for num_threads(NB_THREADS)
    for (unsigned int portion = 0; portion < NB_THREADS; portion++) {
        for (size_t i = portion * PORTION_SIZE; i < (portion + 1) * PORTION_SIZE; i++) {
            glm::vec3 pixel_screen_position = glm::vec3((static_cast<float>(i % WIDTH) / WIDTH), (1.f - (static_cast<float>(i / WIDTH) / HEIGHT)), 0.f);  // y pointing upward
            glm::vec3 sample_screen_position = pixel_screen_position + glm::vec3(frand() / WIDTH, frand() / HEIGHT, 0.f);
            Ray r = camera->getRay(sample_screen_position.x, sample_screen_position.y);
            glm::vec3 sample_color = get_color(r, *world, sample);
            glm::vec3 previous_color(image_buffer[(i * 3)], image_buffer[(i * 3) + 1], image_buffer[(i * 3) + 2]);
            glm::vec3 color = buffer_factor * previous_color + color_factor * sample_color;
            if (color.x > 1) {
                int a = 0;
                a = a;
            }
            image_buffer[(i * 3)] = color.r;
            image_buffer[(i * 3) + 1] = color.g;
            image_buffer[(i * 3) + 2] = color.b;
        }
    }

    std::cout << " took " << (double(std::clock()) - start) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    ++sample;
    return sample <= NB_SAMPLES;
}

const float* RayTracer::getImageBuffer() const
{
    return image_buffer;
}

void RayTracer::setWorld(std::shared_ptr<Hitable> world) {
    this->world = world;
}

void RayTracer::setCamera(std::shared_ptr<Camera> camera) {
    this->camera = camera;
}

int main()
{
    std::clock_t main_clock(std::clock());

    std::clock_t sub_clock(std::clock());
    std::cout << "Initializing window and OpenGL context...";
    Application application;
    application.init();
    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;
    
    sub_clock = std::clock();
    std::cout << "Initializing ray tracer...";
    RayTracer ray_tracer;
    ray_tracer.init();
    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;


    sub_clock = std::clock();
    std::cout << "Loading models data...";

    std::shared_ptr<HitableList> world = std::make_shared<HitableList>();
    std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(0, 0, 0), 1000.f);
    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->texture = std::make_shared<ImageTexture>("lakeside_2k.hdr");
    sphere->material = material;
    world->list.push_back(sphere);
    ray_tracer.setWorld(world);

    float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
    glm::vec3 look_from = glm::vec3(0, 0, -1);
    glm::vec3 look_at = glm::vec3(10, 3, 10);
    float dist_to_focus = 10.f;
    float aperture = 0.f;
    float fov = (40.f * float(M_PI)) / 180.f;
    std::shared_ptr<Camera> camera = std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
    ray_tracer.setCamera(camera);

    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;


    sub_clock = std::clock();
    std::cout << "Setting up world...";
    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;

    sub_clock = std::clock();
    std::cout << "Started ray tracing:" << std::endl;

    while (ray_tracer.iterate()) {
        application.refreshWindow(ray_tracer.getImageBuffer());
    }

    std::cout << "Ray tracing took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    std::cout << "End. Process took " << (double(std::clock()) - main_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    std::cout << "Press Enter to close the application" << std::endl;
    std::cin.ignore();

    return 0;
}

glm::vec3 get_color(const Ray& r, const Hitable& world, size_t depth) {
    HitRecord record;
    if (world.hit(r, record)) {
        if (record.material && record.material->texture) {
            return record.material->texture->color(record.u, record.v, record.position);
        }
        return glm::vec3(record.u, record.v, 0);
    }
    return glm::vec3(0, 0, 0);  // background black
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
