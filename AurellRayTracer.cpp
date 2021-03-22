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
	unsigned int make_box(SceneData& scene, unsigned int material_id, const Transform& t = {})
	{
		std::shared_ptr<Mesh> box_mesh = std::make_shared<Mesh>();

		box_mesh->geometry.push_back({ glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0) });  // 0
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1) });  // 1
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0) });  // 2
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1) });  // 1
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 0), glm::vec3(0, 0, -1), glm::vec2(1, 1) });  // 3
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0) });  // 2

		box_mesh->geometry.push_back({ glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec2(0, 0) });  // 4
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) });  // 6
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 1), glm::vec3(0, 0, 1), glm::vec2(1, 1) });  // 7
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) });  // 6

		box_mesh->geometry.push_back({ glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });  // 0
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });  // 1
		box_mesh->geometry.push_back({ glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });  // 0
		box_mesh->geometry.push_back({ glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });  // 4
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });  // 5

		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec2(1, 0) });  // 2
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 1), glm::vec3(1, 0, 0), glm::vec2(1, 0) });  // 6
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 1), glm::vec3(1, 0, 0), glm::vec2(1, 1) });  // 7
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec2(1, 0) });  // 2
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 1), glm::vec3(1, 0, 0), glm::vec2(1, 1) });  // 7
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 0), glm::vec3(1, 0, 0), glm::vec2(1, 1) });  // 3

		box_mesh->geometry.push_back({ glm::vec3(1, 1, 0), glm::vec3(0, 1, 0), glm::vec2(1, 1) });  // 3
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec2(0, 1) });  // 1
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 0), glm::vec3(0, 1, 0), glm::vec2(1, 1) });  // 3
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 1), glm::vec3(0, 1, 0), glm::vec2(1, 1) });  // 7

		box_mesh->geometry.push_back({ glm::vec3(0, 0, 0), glm::vec3(0, -1, 0), glm::vec2(0, 0) });  // 0
		box_mesh->geometry.push_back({ glm::vec3(0, 0, 1), glm::vec3(0, -1, 0), glm::vec2(0, 0) });  // 4
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, -1, 0), glm::vec2(1, 0) });  // 2
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, -1, 0), glm::vec2(1, 0) });  // 2
		box_mesh->geometry.push_back({ glm::vec3(0, 0, 1), glm::vec3(0, -1, 0), glm::vec2(0, 0) });  // 4
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 1), glm::vec3(0, -1, 0), glm::vec2(1, 0) });  // 6

		for (int i = 0; i < 36; ++i) {
			box_mesh->indices.push_back(i);
		}

		box_mesh->transform(t);

		box_mesh->materialId = material_id;

		return scene.addMesh(box_mesh);
	}

	void cornell_box(SceneData &scene) {
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

		unsigned int material_red_id = scene.addMaterial(material_red);
		unsigned int material_green_id = scene.addMaterial(material_green);
		unsigned int material_white_id = scene.addMaterial(material_white);
		unsigned int material_light_id = scene.addMaterial(material_light);

		// Green plane
		std::shared_ptr<Mesh> green_plane = std::make_shared<Mesh>();
		green_plane->geometry.push_back({ glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec2(0, 0) });
		green_plane->geometry.push_back({ glm::vec3(0, 555, 0), glm::vec3(1, 0, 0), glm::vec2(0, 1) });
		green_plane->geometry.push_back({ glm::vec3(0, 0, 555), glm::vec3(1, 0, 0), glm::vec2(1, 0) });
		green_plane->geometry.push_back({ glm::vec3(0, 555, 555), glm::vec3(1, 0, 0), glm::vec2(1, 1) });
		green_plane->indices = { 0, 1, 2, 3, 2, 1 };
		green_plane->materialId = material_green_id;
		scene.addMesh(green_plane);

		// Red plane
		std::shared_ptr<Mesh> red_plane = std::make_shared<Mesh>();
		red_plane->geometry.push_back({ glm::vec3(555, 0, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });
		red_plane->geometry.push_back({ glm::vec3(555, 0, 555), glm::vec3(-1, 0, 0), glm::vec2(1, 0) });
		red_plane->geometry.push_back({ glm::vec3(555, 555, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });
		red_plane->geometry.push_back({ glm::vec3(555, 555, 555), glm::vec3(-1, 0, 0), glm::vec2(1, 1) });
		red_plane->indices = { 0, 1, 2, 3, 2, 1 };
		red_plane->materialId = material_red_id;
		scene.addMesh(red_plane);

		std::shared_ptr<Mesh> white_ceiling = std::make_shared<Mesh>();
		white_ceiling->geometry.push_back({ glm::vec3(0, 555, 0), glm::vec3(0, -1, 0), glm::vec2(0, 0) });
		white_ceiling->geometry.push_back({ glm::vec3(555, 555, 0), glm::vec3(0, -1, 0), glm::vec2(1, 0) });
		white_ceiling->geometry.push_back({ glm::vec3(0, 555, 555), glm::vec3(0, -1, 0), glm::vec2(0, 1) });
		white_ceiling->geometry.push_back({ glm::vec3(555, 555, 555), glm::vec3(0, -1, 0), glm::vec2(0, 0) });
		white_ceiling->indices = { 0, 1, 2, 3, 2, 1 };
		white_ceiling->materialId = material_white_id;
		scene.addMesh(white_ceiling);

		std::shared_ptr<Mesh> white_plane = std::make_shared<Mesh>();
		white_plane->geometry.push_back({ glm::vec3(0, 0, 555), glm::vec3(0, 0, -1), glm::vec2(0, 0) });
		white_plane->geometry.push_back({ glm::vec3(0, 555, 555), glm::vec3(0, 0, -1), glm::vec2(1, 0) });
		white_plane->geometry.push_back({ glm::vec3(555, 0, 555), glm::vec3(0, 0, -1), glm::vec2(0, 1) });
		white_plane->geometry.push_back({ glm::vec3(555, 555, 555), glm::vec3(0, 0, -1), glm::vec2(0, 0) });
		white_plane->indices = { 0, 1, 2, 3, 2, 1 };
		white_plane->materialId = material_white_id;
		scene.addMesh(white_plane);

		std::shared_ptr<Mesh> white_floor = std::make_shared<Mesh>();
		white_floor->geometry.push_back({ glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec2(0, 0) });
		white_floor->geometry.push_back({ glm::vec3(0, 0, 555), glm::vec3(0, 1, 0), glm::vec2(0, 1) });
		white_floor->geometry.push_back({ glm::vec3(555, 0, 0), glm::vec3(0, 1, 0), glm::vec2(1, 0) });
		white_floor->geometry.push_back({ glm::vec3(555, 0, 555), glm::vec3(0, 1, 0), glm::vec2(0, 0) });
		white_floor->indices = { 0, 1, 2, 3, 2, 1 };
		white_floor->materialId = material_white_id;
		scene.addMesh(white_floor);

		// Light plane
		std::shared_ptr<Triangle> triangle_light_0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(213, 554, 227), glm::vec3(0, -1, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(343, 554, 227), glm::vec3(0, -1, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(213, 554, 332), glm::vec3(0, -1, 0), glm::vec2(0, 1) })
		);
		std::shared_ptr<Triangle> triangle_light_1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(343, 554, 332), glm::vec3(0, -1, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(213, 554, 332), glm::vec3(0, -1, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(343, 554, 227), glm::vec3(0, -1, 0), glm::vec2(1, 0) })
		);
		triangle_light_0->materialId = material_light_id;
		triangle_light_1->materialId = material_light_id;
		scene.addMesh(triangle_light_0);
		scene.addMesh(triangle_light_1);

		Transform cubic_box_transform;
		cubic_box_transform.scaling = glm::vec3(165);
		cubic_box_transform.translation = glm::vec3(265, 0, 65);
		cubic_box_transform.rotation_rads = glm::vec3(0, 18 * (float(M_PI) / 180.f), 0);
		make_box(scene, material_white_id, cubic_box_transform);

		Transform tall_box_transform;
		tall_box_transform.scaling = glm::vec3(165, 330, 165);
		tall_box_transform.translation = glm::vec3(130, 0, 295);
		tall_box_transform.rotation_rads = glm::vec3(0, -15 * (float(M_PI) / 180.f), 0);
		make_box(scene, material_white_id, tall_box_transform);
	}

	std::shared_ptr<Camera> backpack_scene(SceneData &scene)
	{
		Transform t;
		t.translation = glm::vec3(0.5f, 0, 3);
		t.scaling = glm::vec3(1, 1, 1);
		t.rotation_rads = glm::vec3(0, -(3 * M_PI) / 4, 0);
		std::string model_path = "Survival_BackPack_2/backpack.obj";
		if (!ModelLoader::loadModel(model_path, scene, t)) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}

		std::shared_ptr<Material> light_material = std::make_shared<Material>();
		light_material->albedoValue = glm::vec3(0, 0, 0);
		light_material->emissionValue = glm::vec3(100, 100, 100);
		light_material->recomputeBSDF();
		std::shared_ptr<Sphere> light_sphere = std::make_shared<Sphere>(glm::vec3(-1, 1, 4), 0.5f);
		light_sphere->material = light_material;
		unsigned int light_material_id = scene.addMaterial(light_material);

		std::shared_ptr<Triangle> light_mesh0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 0, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) })
			);
		light_mesh0->materialId = light_material_id;
		scene.addMesh(light_mesh0);

		std::shared_ptr<Triangle> light_mesh1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 1, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) })
		);
		light_mesh1->materialId = light_material_id;
		scene.addMesh(light_mesh1);

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		glm::vec3 look_from = glm::vec3(0, 0, -6);
		glm::vec3 look_at = glm::vec3(0, 0, 0);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	std::shared_ptr<Camera> teapot_scene(SceneData &scene)
	{
		Transform t;
		t.translation = glm::vec3(0, 0, 3);
		t.scaling = glm::vec3(0.08f, 0.08f, 0.08f);
		t.rotation_rads = glm::vec3(0, M_PI / 4, 0);
		std::string model_path = "utah-teapot.obj";
		if (!ModelLoader::loadModel(model_path, scene, t)) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}

		std::shared_ptr<Material> light_material = std::make_shared<Material>();
		light_material->albedoValue = glm::vec3(0, 0, 0);
		light_material->emissionValue = glm::vec3(10, 10, 10);
		light_material->recomputeBSDF();
		std::shared_ptr<Sphere> light_sphere = std::make_shared<Sphere>(glm::vec3(-1, 1, 4), 0.5f);
		light_sphere->material = light_material;
		unsigned int light_material_id = scene.addMaterial(light_material);

		std::shared_ptr<Triangle> light_mesh0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 0, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) })
			);
		light_mesh0->materialId = light_material_id;
		scene.addMesh(light_mesh0);

		std::shared_ptr<Triangle> light_mesh1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 1, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) })
			);
		light_mesh1->materialId = light_material_id;
		scene.addMesh(light_mesh1);

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		glm::vec3 look_from = glm::vec3(0, 0, -6);
		glm::vec3 look_at = glm::vec3(0, 0, 0);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	std::shared_ptr<Camera> cerberus_scene(SceneData &scene)
	{
		Transform t;
		t.translation = glm::vec3(0, 0, 200);
		//t.scaling = glm::vec3(0.08f, 0.08f, 0.08f);
		t.rotation_rads = glm::vec3(0, -M_PI / 12, 0);
		std::string model_path = "Cerberus_by_Andrew_Maximov/Cerberus_LP.fbx";
		if (!ModelLoader::loadModel(model_path, scene, t)) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}

		std::shared_ptr<Material> light_material = std::make_shared<Material>();
		light_material->albedoValue = glm::vec3(0, 0, 0);
		light_material->emissionValue = glm::vec3(10000, 10000, 10000);
		light_material->recomputeBSDF();
		std::shared_ptr<Sphere> light_sphere = std::make_shared<Sphere>(glm::vec3(-1, 1, 4), 0.5f);
		light_sphere->material = light_material;
		unsigned int light_material_id = scene.addMaterial(light_material);

		Transform light_transform;
		light_transform.translation = glm::vec3(-30, 0, 100);
		light_transform.scaling = glm::vec3(20, 20, 20);
		std::shared_ptr<Triangle> light_mesh0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(0, -0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(0, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(0, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) })
			);
		light_mesh0->transform(light_transform);
		light_mesh0->materialId = light_material_id;
		scene.addMesh(light_mesh0);

		std::shared_ptr<Triangle> light_mesh1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(0, 0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1) }),
			Vertex({ glm::vec3(0, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(0, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) })
			);
		light_mesh1->transform(light_transform);
		light_mesh1->materialId = light_material_id;
		scene.addMesh(light_mesh1);

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		glm::vec3 look_from = glm::vec3(0, 0, -6);
		glm::vec3 look_at = glm::vec3(0, 0, 0);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	std::shared_ptr<Camera> cornell_box_scene(SceneData& scene) {
		cornell_box(scene);

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		glm::vec3 look_from = glm::vec3(278, 278, -800);  // Cornell box
		glm::vec3 look_at = glm::vec3(278, 278, 0);  // Cornell box
		float dist_to_focus = 10.f;  // Cornell box
		float aperture = 0.f;  // Cornell box
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}
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

	std::shared_ptr<Camera> camera = cornell_box_scene(*scene);

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
	for (int y = 0; y < Application::HEIGHT; ++y) {
		for (int x = 0; x < Application::WIDTH * 3; x+=3) {
			file_stream << image_buffer[y * Application::WIDTH * 3 + x] << " ";
			file_stream << image_buffer[y * Application::WIDTH * 3 + x + 1] << " ";
			file_stream << image_buffer[y * Application::WIDTH * 3 + x + 2] << std::endl;
		}
	}
	file_stream.close();

    std::cout << "Ray tracing took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;
	
	std::cout << "End. Process took " << (double(std::clock()) - main_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    application.idle();

    return 0;
}

