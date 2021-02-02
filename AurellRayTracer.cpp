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
#include "BVHBuilder.h"
#include "ModelLoader.h"
#include "Triangle.h"


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

    std::shared_ptr<Triangle> triangle0 = std::make_shared<Triangle>(
        Vertex(glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(1, 1, 1), glm::vec2(1, 1), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(0, 1, 1), glm::vec2(0, 1), glm::vec3(1, 0, 0))
    );
    std::shared_ptr<Triangle> triangle1 = std::make_shared<Triangle>(
        Vertex(glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(1, 0, 1), glm::vec2(1, 0), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(1, 1, 1), glm::vec2(1, 1), glm::vec3(1, 0, 0))
    );
    //world->list.push_back(triangle0);
    //world->list.push_back(triangle1);
    triangle0->transform(glm::vec3(-0.5, -0.5, -0.5));
    triangle1->transform(glm::vec3(-0.5, -0.5, -0.5));
    std::shared_ptr<Material> material2 = std::make_shared<Material>();
    //material2->diffuse = std::make_shared<ImageTexture>("uvs.png");
    triangle0->material = material2;
    triangle1->material = material2;
    HitRecord record;
    //triangle0->hit(Ray(glm::vec3(0, 0, 0), glm::normalize(glm::vec3(-0.45, 0.45, 0.5))), 0, 100000, record);
    triangle1->hit(Ray(glm::vec3(0, 0, 0), glm::normalize(glm::vec3(0.05, 0, 0.5))), 0, 100000, record);
    
    std::shared_ptr<HitableList> model;
    //model = ModelLoader::loadModel("utah-teapot.obj");
    model = ModelLoader::loadModel("Survival_BackPack_2/backpack.obj");
    //model = ModelLoader::loadModel("Cerberus_by_Andrew_Maximov/Cerberus_LP.fbx");
    if (model) {
        //model->transform(glm::vec3(0, 0, 50));  // teapot
        model->transform(glm::vec3(-0.5f, 0, 6), glm::vec3(0, M_PI / 5 * 4, 0));  // backpack
        //model->transform(glm::vec3(50, 10, 200), glm::vec3(0, -M_PI / 4, 0));  // cerberus

        world->concatenateHitableList(model);
    }

   //world->list.push_back(std::make_shared<Sphere>(glm::vec3(0, 0, 10), 1.f));

    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;

    sub_clock = std::clock();
    float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
    glm::vec3 look_from = glm::vec3(0, 0, -1);
    glm::vec3 look_at = glm::vec3(0, 0, 10);
    float dist_to_focus = 10.f;
    float aperture = 0.f;
    float fov = (40.f * float(M_PI)) / 180.f;
    std::shared_ptr<Camera> camera = std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
    ray_tracer.setCamera(camera);

    std::cout << "Computing BVH...";

    std::shared_ptr<BVHNode> bvh = BVHBuilder::buildBVH(world->list, 0, world->list.size(), 0.f, 1.f);
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

