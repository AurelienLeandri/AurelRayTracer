// AurellRayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <ctime>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Camera.h"
#include "Sphere.h"
#include "HitableList.h"
#include "ImageTexture.h"
#include "Application.h"
#include "RayTracer.h"
#include "BVHNode.h"
#include "ModelLoader.h"


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
    material->diffuse = std::make_shared<ImageTexture>("lakeside_2k.hdr");
    sphere->material = material;
    world->list.push_back(sphere);

    std::shared_ptr<HitableList> utah_teapot = ModelLoader::loadModel("utah-teapot.obj");
    utah_teapot->transform(glm::vec3(0, 0, 50));
    world->concatenateHitableList(utah_teapot);

   //world->list.push_back(std::make_shared<Sphere>(glm::vec3(0, 0, 10), 1.f));

    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;


    sub_clock = std::clock();
    std::cout << "Setting up world...";
    float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
    glm::vec3 look_from = glm::vec3(0, 0, -1);
    glm::vec3 look_at = glm::vec3(0, 0, 10);
    float dist_to_focus = 10.f;
    float aperture = 0.f;
    float fov = (40.f * float(M_PI)) / 180.f;
    std::shared_ptr<Camera> camera = std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
    ray_tracer.setCamera(camera);

    std::shared_ptr<BVHNode> bvh = std::make_shared<BVHNode>(world->list, 0, world->list.size(), 0.f, 1.f);
    ray_tracer.setWorld(bvh);

    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;

    sub_clock = std::clock();
    std::cout << "Started ray tracing:" << std::endl;

    bool finished = false;
    do {
        finished = ray_tracer.iterate();
        application.refreshWindow(ray_tracer.getImageBuffer());
    } while (!finished);

    std::cout << "Ray tracing took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    std::cout << "End. Process took " << (double(std::clock()) - main_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    application.idle();

    return 0;
}

