// AurellRayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>

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
#include "SceneData.h"
#include "Triangle.h"
#include "Mesh.h"


namespace {
	/*
	void make_box(std::shared_ptr<HitableList>& hitable_list, const glm::vec3& p0, const glm::vec3& p1,
		bool interior, std::shared_ptr<Material> material, const glm::vec3& translation = glm::vec3(), const glm::vec3& rotation = glm::vec3())
	{
		int f = interior ? 1 : -1;

		// Right plane
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p1.x, p0.y, p0.z), glm::vec2(0, 0), glm::vec3(-f, 0, 0)),
			Vertex(interior ? glm::vec3(p1.x, p0.y, p1.z) : glm::vec3(p1.x, p1.y, p0.z), glm::vec2(f, 0), glm::vec3(-f, 0, 0)),
			Vertex(interior ? glm::vec3(p1.x, p1.y, p0.z) : glm::vec3(p1.x, p0.y, p1.z), glm::vec2(0, f), glm::vec3(-f, 0, 0)),
			material, translation, rotation
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p1.x, p1.y, p1.z), glm::vec2(0, 0), glm::vec3(-f, 0, 0)),
			Vertex(interior ? glm::vec3(p1.x, p1.y, p0.z) : glm::vec3(p1.x, p0.y, p1.z), glm::vec2(f, 0), glm::vec3(-f, 0, 0)),
			Vertex(interior ? glm::vec3(p1.x, p0.y, p1.z) : glm::vec3(p1.x, p1.y, p0.z), glm::vec2(0, f), glm::vec3(-f, 0, 0)),
			material, translation, rotation
			));

		// Left plane
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p0.x, p0.y, p0.z), glm::vec2(0, 0), glm::vec3(f, 0, 0)),
			Vertex(interior ? glm::vec3(p0.x, p1.y, p0.z) : glm::vec3(p0.x, p0.y, p1.z), glm::vec2(0, f), glm::vec3(f, 0, 0)),
			Vertex(interior ? glm::vec3(p0.x, p0.y, p1.z) : glm::vec3(p0.x, p1.y, p0.z), glm::vec2(f, 0), glm::vec3(f, 0, 0)),
			material, translation, rotation
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p0.x, p1.y, p1.z), glm::vec2(0, 0), glm::vec3(f, 0, 0)),
			Vertex(interior ? glm::vec3(p0.x, p0.y, p1.z) : glm::vec3(p0.x, p1.y, p0.z), glm::vec2(0, f), glm::vec3(f, 0, 0)),
			Vertex(interior ? glm::vec3(p0.x, p1.y, p0.z) : glm::vec3(p0.x, p0.y, p1.z), glm::vec2(f, 0), glm::vec3(f, 0, 0)),
			material, translation, rotation
			));

		// Top plane
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p0.x, p1.y, p0.z), glm::vec2(0, 0), glm::vec3(0, -f, 0)),
			Vertex(interior ? glm::vec3(p1.x, p1.y, p0.z) : glm::vec3(p0.x, p1.y, p1.z), glm::vec2(f, 0), glm::vec3(0, -f, 0)),
			Vertex(interior ? glm::vec3(p0.x, p1.y, p1.z) : glm::vec3(p1.x, p1.y, p0.z), glm::vec2(0, f), glm::vec3(0, -f, 0)),
			material, translation, rotation
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p1.x, p1.y, p1.z), glm::vec2(0, 0), glm::vec3(0, -f, 0)),
			Vertex(interior ? glm::vec3(p0.x, p1.y, p1.z) : glm::vec3(p1.x, p1.y, p0.z), glm::vec2(0, f), glm::vec3(0, -f, 0)),
			Vertex(interior ? glm::vec3(p1.x, p1.y, p0.z) : glm::vec3(p0.x, p1.y, p1.z), glm::vec2(f, 0), glm::vec3(0, -f, 0)),
			material, translation, rotation
			));

		// Bottom plane
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p0.x, p0.y, p0.z), glm::vec2(0, 0), glm::vec3(0, f, 0)),
			Vertex(interior ? glm::vec3(p0.x, p0.y, p1.z) : glm::vec3(p1.x, p0.y, p0.z), glm::vec2(0, f), glm::vec3(0, f, 0)),
			Vertex(interior ? glm::vec3(p1.x, p0.y, p0.z) : glm::vec3(p0.x, p0.y, p1.z), glm::vec2(f, 0), glm::vec3(0, f, 0)),
			material, translation, rotation
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p1.x, p0.y, p1.z), glm::vec2(0, 0), glm::vec3(0, f, 0)),
			Vertex(interior ? glm::vec3(p1.x, p0.y, p0.z) : glm::vec3(p0.x, p0.y, p1.z), glm::vec2(f, 0), glm::vec3(0, f, 0)),
			Vertex(interior ? glm::vec3(p0.x, p0.y, p1.z) : glm::vec3(p1.x, p0.y, p0.z), glm::vec2(0, f), glm::vec3(0, f, 0)),
			material, translation, rotation
			));


		// Back plane
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p0.x, p0.y, p1.z), glm::vec2(0, 0), glm::vec3(0, 0, -f)),
			Vertex(interior ? glm::vec3(p0.x, p1.y, p1.z) : glm::vec3(p1.x, p0.y, p1.z), glm::vec2(f, 0), glm::vec3(0, 0, -f)),
			Vertex(interior ? glm::vec3(p1.x, p0.y, p1.z) : glm::vec3(p0.x, p1.y, p1.z), glm::vec2(0, f), glm::vec3(0, 0, -f)),
			material, translation, rotation
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p1.x, p1.y, p1.z), glm::vec2(0, 0), glm::vec3(0, 0, -f)),
			Vertex(interior ? glm::vec3(p1.x, p0.y, p1.z) : glm::vec3(p0.x, p1.y, p1.z), glm::vec2(0, f), glm::vec3(0, 0, -f)),
			Vertex(interior ? glm::vec3(p0.x, p1.y, p1.z) : glm::vec3(p1.x, p0.y, p1.z), glm::vec2(f, 0), glm::vec3(0, 0, -f)),
			material, translation, rotation
			));

		// Front plane
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p0.x, p0.y, p0.z), glm::vec2(0, 0), glm::vec3(0, 0, f)),
			Vertex(interior ? glm::vec3(p1.x, p0.y, p0.z) : glm::vec3(p0.x, p1.y, p0.z), glm::vec2(0, f), glm::vec3(0, 0, f)),
			Vertex(interior ? glm::vec3(p0.x, p1.y, p0.z) : glm::vec3(p1.x, p0.y, p0.z), glm::vec2(f, 0), glm::vec3(0, 0, f)),
			material, translation, rotation
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(p1.x, p1.y, p0.z), glm::vec2(0, 0), glm::vec3(0, 0, f)),
			Vertex(interior ? glm::vec3(p0.x, p1.y, p0.z) : glm::vec3(p1.x, p0.y, p0.z), glm::vec2(f, 0), glm::vec3(0, 0, f)),
			Vertex(interior ? glm::vec3(p1.x, p0.y, p0.z) : glm::vec3(p0.x, p1.y, p0.z), glm::vec2(0, f), glm::vec3(0, 0, f)),
			material, translation, rotation
			));
	}

	std::shared_ptr<HitableList> cornell_box(std::vector<std::shared_ptr<Hitable>> &lights) {
		std::shared_ptr<HitableList> hitable_list = std::make_shared<HitableList>();
		std::shared_ptr<Material> material_red = std::make_shared<Material>();
		material_red->albedoValue = glm::vec3(0.65f, 0.05f, 0.05f);
		std::shared_ptr<Material> material_white = std::make_shared<Material>();
		material_white->albedoValue = glm::vec3(0.73f, 0.73f, 0.73f);
		std::shared_ptr<Material> material_green = std::make_shared<Material>();
		material_green->albedoValue = glm::vec3(0.12f, 0.45f, 0.15f);
		std::shared_ptr<Material> material_light = std::make_shared<Material>();
		material_light->emissionValue = glm::vec3(30, 30, 30);
		material_light->albedoValue = glm::vec3(0, 0, 0);

		material_red->recomputeBSDF();
		material_white->recomputeBSDF();
		material_green->recomputeBSDF();
		material_light->recomputeBSDF();

		// Green plane
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(555, 0, 0), glm::vec2(0, 0), glm::vec3(-1, 0, 0)),
			Vertex(glm::vec3(555, 0, 555), glm::vec2(1, 0), glm::vec3(-1, 0, 0)),
			Vertex(glm::vec3(555, 555, 0), glm::vec2(0, 1), glm::vec3(-1, 0, 0)),
			material_red
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(555, 555, 555), glm::vec2(0, 0), glm::vec3(-1, 0, 0)),
			Vertex(glm::vec3(555, 555, 0), glm::vec2(1, 0), glm::vec3(-1, 0, 0)),
			Vertex(glm::vec3(555, 0, 555), glm::vec2(0, 1), glm::vec3(-1, 0, 0)),
			material_red
			));

		// Red plane
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(0, 0, 0), glm::vec2(0, 0), glm::vec3(1, 0, 0)),
			Vertex(glm::vec3(0, 555, 0), glm::vec2(0, 1), glm::vec3(1, 0, 0)),
			Vertex(glm::vec3(0, 0, 555), glm::vec2(1, 0), glm::vec3(1, 0, 0)),
			material_green
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(0, 555, 555), glm::vec2(0, 0), glm::vec3(1, 0, 0)),
			Vertex(glm::vec3(0, 0, 555), glm::vec2(0, 1), glm::vec3(1, 0, 0)),
			Vertex(glm::vec3(0, 555, 0), glm::vec2(1, 0), glm::vec3(1, 0, 0)),
			material_green
			));

		// White ceiling
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(0, 555, 0), glm::vec2(0, 0), glm::vec3(0, -1, 0)),
			Vertex(glm::vec3(555, 555, 0), glm::vec2(1, 0), glm::vec3(0, -1, 0)),
			Vertex(glm::vec3(0, 555, 555), glm::vec2(0, 1), glm::vec3(0, -1, 0)),
			material_white
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(555, 555, 555), glm::vec2(0, 0), glm::vec3(0, -1, 0)),
			Vertex(glm::vec3(0, 555, 555), glm::vec2(0, 1), glm::vec3(0, -1, 0)),
			Vertex(glm::vec3(555, 555, 0), glm::vec2(1, 0), glm::vec3(0, -1, 0)),
			material_white
			));

		// White wall in the back
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(0, 0, 555), glm::vec2(0, 0), glm::vec3(0, 0, -1)),
			Vertex(glm::vec3(0, 555, 555), glm::vec2(1, 0), glm::vec3(0, 0, -1)),
			Vertex(glm::vec3(555, 0, 555), glm::vec2(0, 1), glm::vec3(0, 0, -1)),
			material_white
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(555, 555, 555), glm::vec2(0, 0), glm::vec3(0, 0, -1)),
			Vertex(glm::vec3(555, 0, 555), glm::vec2(0, 1), glm::vec3(0, 0, -1)),
			Vertex(glm::vec3(0, 555, 555), glm::vec2(1, 0), glm::vec3(0, 0, -1)),
			material_white
			));

		// White floor
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(0, 0, 0), glm::vec2(0, 0), glm::vec3(0, 1, 0)),
			Vertex(glm::vec3(0, 0, 555), glm::vec2(0, 1), glm::vec3(0, 1, 0)),
			Vertex(glm::vec3(555, 0, 0), glm::vec2(1, 0), glm::vec3(0, 1, 0)),
			material_white
			));
		hitable_list->list.push_back(std::make_shared<Triangle>(
			Vertex(glm::vec3(555, 0, 555), glm::vec2(0, 0), glm::vec3(0, 1, 0)),
			Vertex(glm::vec3(555, 0, 0), glm::vec2(1, 0), glm::vec3(0, 1, 0)),
			Vertex(glm::vec3(0, 0, 555), glm::vec2(0, 1), glm::vec3(0, 1, 0)),
			material_white
			));

		// Light plane
		std::shared_ptr<Triangle> triangle_light_1 = std::make_shared<Triangle>(
			Vertex(glm::vec3(213, 554, 227), glm::vec2(0, 0), glm::vec3(0, -1, 0)),
			Vertex(glm::vec3(343, 554, 227), glm::vec2(1, 0), glm::vec3(0, -1, 0)),
			Vertex(glm::vec3(213, 554, 332), glm::vec2(0, 1), glm::vec3(0, -1, 0)),
			material_light
			);
		std::shared_ptr<Triangle> triangle_light_2 = std::make_shared<Triangle>(
			Vertex(glm::vec3(343, 554, 332), glm::vec2(0, 0), glm::vec3(0, -1, 0)),
			Vertex(glm::vec3(213, 554, 332), glm::vec2(0, 1), glm::vec3(0, -1, 0)),
			Vertex(glm::vec3(343, 554, 227), glm::vec2(1, 0), glm::vec3(0, -1, 0)),
			material_light
			);
		hitable_list->list.push_back(triangle_light_1);
		hitable_list->list.push_back(triangle_light_2);

		lights.push_back(triangle_light_1);
		lights.push_back(triangle_light_2);

		// Cubic box
		make_box(hitable_list, glm::vec3(-165 / 2, -165 / 2, -165 / 2), glm::vec3(165 / 2, 165 / 2, 165 / 2), false,
			material_white, glm::vec3(165 / 2 + 130, 165 / 2, 165 / 2 + 65), glm::vec3(0, 18 * (float(M_PI) / 180.f), 0));

		// Tall box
		make_box(hitable_list, glm::vec3(-165 / 2, -330 / 2, -165 / 2), glm::vec3(165 / 2, 330 / 2, 165 / 2), false,
			material_white, glm::vec3(165 / 2 + 265, 330 / 2, 165 / 2 + 295), glm::vec3(0, -15 * (float(M_PI) / 180.f), 0));
		return hitable_list;
	}
	*/

	std::shared_ptr<Camera> backpack_scene(SceneData &scene)
	{
		std::string model_path = "Survival_BackPack_2/backpack.obj";
		if (!ModelLoader::loadModel(model_path, scene)) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}

		std::shared_ptr<Material> light_material = std::make_shared<Material>();
		light_material->albedoValue = glm::vec3(0, 0, 0);
		light_material->emissionValue = glm::vec3(1000, 1000, 1000);
		light_material->recomputeBSDF();
		std::shared_ptr<Sphere> light_sphere = std::make_shared<Sphere>(glm::vec3(-1, 1, 4), 0.5f);
		light_sphere->material = light_material;
		unsigned int light_material_id = scene.addMaterial(light_material);

		std::shared_ptr<Triangle> light_mesh0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 0, 6), glm::vec3(1, 0, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(-3, 1, 6), glm::vec3(1, 0, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(-3, 0, 7), glm::vec3(1, 0, 0), glm::vec2(1, 0) })
			);
		light_mesh0->materialId = light_material_id;
		scene.addMesh(light_mesh0);

		std::shared_ptr<Triangle> light_mesh1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 1, 7), glm::vec3(1, 0, 0), glm::vec2(1, 1) }),
			Vertex({ glm::vec3(-3, 0, 7), glm::vec3(1, 0, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(-3, 1, 6), glm::vec3(1, 0, 0), glm::vec2(0, 1) })
		);
		light_mesh1->materialId = light_material_id;
		scene.addMesh(light_mesh1);

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		glm::vec3 look_from = glm::vec3(5, 0, 6);
		glm::vec3 look_at = glm::vec3(0, 0, 0);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	std::shared_ptr<Camera> teapot_scene(SceneData &scene)
	{
		std::string model_path = "utah-teapot.obj";
		if (!ModelLoader::loadModel(model_path, scene)) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}

		/*
		std::shared_ptr<Material> light_material = std::make_shared<Material>();
		light_material->albedoValue = glm::vec3(0, 0, 0);
		light_material->emissionValue = glm::vec3(1000, 1000, 1000);
		light_material->recomputeBSDF();
		std::shared_ptr<Sphere> light_sphere = std::make_shared<Sphere>(glm::vec3(-1, 1, 4), 0.5f);
		light_sphere->material = light_material;
		world->list.push_back(light_sphere);

		lights.push_back(light_sphere);
		*/

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		glm::vec3 look_from = glm::vec3(0, 0, -50);
		glm::vec3 look_at = glm::vec3(0, 0, 10);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	std::shared_ptr<Camera> cerberus_scene(SceneData &scene)
	{
		std::string model_path = "Cerberus_by_Andrew_Maximov/Cerberus_LP.fbx";
		if (!ModelLoader::loadModel(model_path, scene)) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}
		//model->transform(glm::vec3(50, 10, 200), glm::vec3(0, -M_PI / 4, 0));  // cerberus

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		glm::vec3 look_from = glm::vec3(-50, -10, -200);
		glm::vec3 look_at = glm::vec3(0, 0, 10);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	/*
	std::shared_ptr<Camera> cornell_box_scene(std::shared_ptr<HitableList> world, std::vector<std::shared_ptr<Hitable>>& lights) {
		std::shared_ptr<HitableList> cornell = cornell_box(lights);
		world->concatenateHitableList(cornell);

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		glm::vec3 look_from = glm::vec3(278, 278, -800);  // Cornell box
		glm::vec3 look_at = glm::vec3(278, 278, 0);  // Cornell box
		float dist_to_focus = 10.f;  // Cornell box
		float aperture = 0.f;  // Cornell box
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}
	*/
}

int main()
{
	//test_interpolate();
	//return 0;

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
	std::cout << "Loading scene data...";

	SceneData* scene = SceneFactory::createScene();

	std::shared_ptr<Camera> camera = backpack_scene(*scene);

	ray_tracer.setCamera(camera);
	ray_tracer.setScene(*scene);

    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;

    sub_clock = std::clock();
    std::cout << "Started ray tracing:" << std::endl;

	ray_tracer.start();

    bool finished = false;
    do {
        finished = ray_tracer.iterate();
        application.refreshWindow(ray_tracer.getImageBuffer());
    } while (!finished);
	
	std::fstream file_stream;
	file_stream.open("image_buffer.txt", std::ios::out);
	const float* image_buffer = ray_tracer.getImageBuffer();
	for (int y = 200; y < 600; ++y) {
		for (int x = 0; x < 800 * 3; x+=3) {
			file_stream << image_buffer[y * 800 * 3 + x] << " ";
			file_stream << image_buffer[y * 800 * 3 + x + 1] << " ";
			file_stream << image_buffer[y * 800 * 3 + x + 2] << std::endl;
		}
	}
	file_stream.close();

    std::cout << "Ray tracing took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;
	
	std::cout << "End. Process took " << (double(std::clock()) - main_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    application.idle();

    return 0;
}

