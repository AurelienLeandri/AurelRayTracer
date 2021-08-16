// AurellRayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>

#include "Camera.h"
#include "Texture.h"
#include "ApplicationOld.h"
#include "PathTracer.h"
#include "ModelLoader.h"
#include "Scene.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Texture.h"
#include "Distribution2D.h"
#include "Utils.h"
#include "Material.h"
#include "Sphere.h"
#include "Light.h"
#include "Application.h"


namespace {
	int make_box(Scene& scene, int material_id, const Transform& t = {})
	{
		std::shared_ptr<Mesh> box_mesh = std::make_shared<Mesh>();
		std::vector<Vertex>& vertices = box_mesh->getVertices();

		vertices.push_back({ glm::vec3(0), glm::vec3(0, 0, -1), glm::vec2(0, 0) });  // 0
		vertices.push_back({ glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1) });  // 1
		vertices.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0) });  // 2
		vertices.push_back({ glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1) });  // 1
		vertices.push_back({ glm::vec3(1, 1, 0), glm::vec3(0, 0, -1), glm::vec2(1, 1) });  // 3
		vertices.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0) });  // 2

		vertices.push_back({ glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec2(0, 0) });  // 4
		vertices.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1) });  // 5
		vertices.push_back({ glm::vec3(1, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) });  // 6
		vertices.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1) });  // 5
		vertices.push_back({ glm::vec3(1), glm::vec3(0, 0, 1), glm::vec2(1, 1) });  // 7
		vertices.push_back({ glm::vec3(1, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) });  // 6

		vertices.push_back({ glm::vec3(0), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });  // 0
		vertices.push_back({ glm::vec3(0, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });  // 5
		vertices.push_back({ glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });  // 1
		vertices.push_back({ glm::vec3(0), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });  // 0
		vertices.push_back({ glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });  // 4
		vertices.push_back({ glm::vec3(0, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });  // 5

		vertices.push_back({ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec2(1, 0) });  // 2
		vertices.push_back({ glm::vec3(1, 0, 1), glm::vec3(1, 0, 0), glm::vec2(1, 0) });  // 6
		vertices.push_back({ glm::vec3(1), glm::vec3(1, 0, 0), glm::vec2(1, 1) });  // 7
		vertices.push_back({ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec2(1, 0) });  // 2
		vertices.push_back({ glm::vec3(1), glm::vec3(1, 0, 0), glm::vec2(1, 1) });  // 7
		vertices.push_back({ glm::vec3(1, 1, 0), glm::vec3(1, 0, 0), glm::vec2(1, 1) });  // 3

		vertices.push_back({ glm::vec3(1, 1, 0), glm::vec3(0, 1, 0), glm::vec2(1, 1) });  // 3
		vertices.push_back({ glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec2(0, 1) });  // 1
		vertices.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1) });  // 5
		vertices.push_back({ glm::vec3(1, 1, 0), glm::vec3(0, 1, 0), glm::vec2(1, 1) });  // 3
		vertices.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1) });  // 5
		vertices.push_back({ glm::vec3(1), glm::vec3(0, 1, 0), glm::vec2(1, 1) });  // 7

		vertices.push_back({ glm::vec3(0), glm::vec3(0, -1, 0), glm::vec2(0, 0) });  // 0
		vertices.push_back({ glm::vec3(0, 0, 1), glm::vec3(0, -1, 0), glm::vec2(0, 0) });  // 4
		vertices.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, -1, 0), glm::vec2(1, 0) });  // 2
		vertices.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, -1, 0), glm::vec2(1, 0) });  // 2
		vertices.push_back({ glm::vec3(0, 0, 1), glm::vec3(0, -1, 0), glm::vec2(0, 0) });  // 4
		vertices.push_back({ glm::vec3(1, 0, 1), glm::vec3(0, -1, 0), glm::vec2(1, 0) });  // 6

		std::vector<int>& indices = box_mesh->getIndices();
		for (int i = 0; i < 36; ++i) {
			indices.push_back(i);
		}

		box_mesh->transform(t);

		box_mesh->setMaterialId(material_id);

		return scene.addShape(box_mesh);
	}

	void cornell_box(Scene &scene) {
		std::shared_ptr<PerfectDiffuseMaterial> material_red = std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.65f, 0.05f, 0.05f)));
		std::shared_ptr<PerfectDiffuseMaterial> material_white = std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.73f, 0.73f, 0.73f)));
		std::shared_ptr<PerfectDiffuseMaterial> material_green = std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.12f, 0.45f, 0.15f)));
		std::shared_ptr<EmissiveMaterial> material_light = std::make_shared<EmissiveMaterial>(std::make_shared<ConstantTexture>(glm::vec3(30, 30, 30)), ConstantTexture::black);

		int material_red_id = scene.addMaterial(material_red);
		int material_green_id = scene.addMaterial(material_green);
		int material_white_id = scene.addMaterial(material_white);
		int material_light_id = scene.addMaterial(material_light);

		std::shared_ptr<DielectricMaterial> fresnel_material = std::make_shared<DielectricMaterial>(1.05f);
		int fresnel_material_id = scene.addMaterial(fresnel_material);

		std::shared_ptr<PerfectSpecularMaterial> mirror_material = std::make_shared<PerfectSpecularMaterial>();
		int mirror_material_id = scene.addMaterial(mirror_material);

		// Green plane
		std::shared_ptr<Mesh> green_plane = std::make_shared<Mesh>();
		green_plane->getVertices().push_back({ glm::vec3(0), glm::vec3(1, 0, 0), glm::vec2(0, 0) });
		green_plane->getVertices().push_back({ glm::vec3(0, 555, 0), glm::vec3(1, 0, 0), glm::vec2(0, 1) });
		green_plane->getVertices().push_back({ glm::vec3(0, 0, 555), glm::vec3(1, 0, 0), glm::vec2(1, 0) });
		green_plane->getVertices().push_back({ glm::vec3(0, 555, 555), glm::vec3(1, 0, 0), glm::vec2(1, 1) });
		green_plane->getIndices() = { 0, 1, 2, 3, 2, 1 };
		green_plane->setMaterialId(fresnel_material_id);
		scene.addShape(green_plane);

		// Red plane
		std::shared_ptr<Mesh> red_plane = std::make_shared<Mesh>();
		red_plane->getVertices().push_back({ glm::vec3(555, 0, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });
		red_plane->getVertices().push_back({ glm::vec3(555, 0, 555), glm::vec3(-1, 0, 0), glm::vec2(1, 0) });
		red_plane->getVertices().push_back({ glm::vec3(555, 555, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });
		red_plane->getVertices().push_back({ glm::vec3(555, 555, 555), glm::vec3(-1, 0, 0), glm::vec2(1, 1) });
		red_plane->getIndices() = { 0, 1, 2, 3, 2, 1 };
		red_plane->setMaterialId(fresnel_material_id);
		scene.addShape(red_plane);

		// White ceiling
		std::shared_ptr<Mesh> white_ceiling = std::make_shared<Mesh>();
		white_ceiling->getVertices().push_back({ glm::vec3(0, 555, 0), glm::vec3(0, -1, 0), glm::vec2(0, 0) });
		white_ceiling->getVertices().push_back({ glm::vec3(555, 555, 0), glm::vec3(0, -1, 0), glm::vec2(1, 0) });
		white_ceiling->getVertices().push_back({ glm::vec3(0, 555, 555), glm::vec3(0, -1, 0), glm::vec2(0, 1) });
		white_ceiling->getVertices().push_back({ glm::vec3(555, 555, 555), glm::vec3(0, -1, 0), glm::vec2(0, 0) });
		white_ceiling->getIndices() = { 0, 1, 2, 3, 2, 1 };
		white_ceiling->setMaterialId(fresnel_material_id);
		scene.addShape(white_ceiling);

		// White plane in the back
		std::shared_ptr<Mesh> white_plane = std::make_shared<Mesh>();
		white_plane->getVertices().push_back({ glm::vec3(0, 0, 555), glm::vec3(0, 0, -1), glm::vec2(0, 0) });
		white_plane->getVertices().push_back({ glm::vec3(0, 555, 555), glm::vec3(0, 0, -1), glm::vec2(1, 0) });
		white_plane->getVertices().push_back({ glm::vec3(555, 0, 555), glm::vec3(0, 0, -1), glm::vec2(0, 1) });
		white_plane->getVertices().push_back({ glm::vec3(555, 555, 555), glm::vec3(0, 0, -1), glm::vec2(0, 0) });
		white_plane->getIndices() = { 0, 1, 2, 3, 2, 1 };
		white_plane->setMaterialId(fresnel_material_id);
		scene.addShape(white_plane);

		// White floor
		std::shared_ptr<Mesh> white_floor = std::make_shared<Mesh>();
		white_floor->getVertices().push_back({ glm::vec3(0), glm::vec3(0, 1, 0), glm::vec2(0, 0) });
		white_floor->getVertices().push_back({ glm::vec3(0, 0, 555), glm::vec3(0, 1, 0), glm::vec2(0, 1) });
		white_floor->getVertices().push_back({ glm::vec3(555, 0, 0), glm::vec3(0, 1, 0), glm::vec2(1, 0) });
		white_floor->getVertices().push_back({ glm::vec3(555, 0, 555), glm::vec3(0, 1, 0), glm::vec2(0, 0) });
		white_floor->getIndices() = { 0, 1, 2, 3, 2, 1 };
		white_floor->setMaterialId(material_white_id);
		scene.addShape(white_floor);

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
		triangle_light_0->setMaterialId(material_light_id);
		triangle_light_1->setMaterialId(material_light_id);
		std::shared_ptr<AreaLight> light0 = std::make_shared<AreaLight>(glm::vec3(30, 30, 30), triangle_light_0);
		scene.addLight(light0, triangle_light_0);
		std::shared_ptr<AreaLight> light1 = std::make_shared<AreaLight>(glm::vec3(30, 30, 30), triangle_light_1);
		scene.addLight(light1, triangle_light_1);

		// Small box
		TransformParameters cubic_box_transform;
		cubic_box_transform.scaling = glm::vec3(165);
		cubic_box_transform.translation = glm::vec3(265, 0, 65);
		cubic_box_transform.rotation_rads = glm::vec3(0, 18 * (float(M_PI) / 180.f), 0);
		make_box(scene, fresnel_material_id, Transform(cubic_box_transform));
		
		// Big box
		TransformParameters tall_box_transform;
		tall_box_transform.scaling = glm::vec3(165, 330, 165);
		tall_box_transform.translation = glm::vec3(130, 0, 295);
		tall_box_transform.rotation_rads = glm::vec3(0, -15 * (float(M_PI) / 180.f), 0);
		make_box(scene, fresnel_material_id, Transform(tall_box_transform));
	}

	std::shared_ptr<Camera> backpack_scene(Scene &scene)
	{
		TransformParameters t;
		t.translation = glm::vec3(0.5f, 0, 3);
		t.scaling = glm::vec3(1);
		t.rotation_rads = glm::vec3(0, -(3 * M_PI) / 4, 0);
		std::string model_path = "Survival_BackPack_2/backpack.obj";
		if (!ModelLoader::loadModel(model_path, scene, Transform(t))) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}

		std::shared_ptr<EmissiveMaterial> light_material = std::make_shared<EmissiveMaterial>(std::make_shared<ConstantTexture>(glm::vec3(100, 100, 100)), ConstantTexture::black);
		int light_material_id = scene.addMaterial(light_material);

		std::shared_ptr<Triangle> light_mesh0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 0, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) })
			);
		light_mesh0->setMaterialId(light_material_id);
		std::shared_ptr<AreaLight> light0 = std::make_shared<AreaLight>(glm::vec3(100, 100, 100), light_mesh0);
		scene.addLight(light0, light_mesh0);

		std::shared_ptr<Triangle> light_mesh1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 1, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) })
		);
		light_mesh1->setMaterialId(light_material_id);
		std::shared_ptr<AreaLight> light1 = std::make_shared<AreaLight>(glm::vec3(100, 100, 100), light_mesh1);
		scene.addLight(light1, light_mesh1);

		float aspect_ratio = static_cast<float>(ApplicationOld::WIDTH) / ApplicationOld::HEIGHT;
		glm::vec3 look_from = glm::vec3(0, 0, -6);
		glm::vec3 look_at = glm::vec3(0);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	std::shared_ptr<Camera> teapot_scene(Scene &scene)
	{
		TransformParameters t;
		t.translation = glm::vec3(0, 0, 3);
		t.scaling = glm::vec3(0.08f, 0.08f, 0.08f);
		t.rotation_rads = glm::vec3(0, M_PI / 4, 0);
		std::string model_path = "utah-teapot.obj";
		if (!ModelLoader::loadModel(model_path, scene, Transform(t))) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}

		std::shared_ptr<EmissiveMaterial> light_material = std::make_shared<EmissiveMaterial>(std::make_shared<ConstantTexture>(glm::vec3(10, 10, 10)), ConstantTexture::black);
		int light_material_id = scene.addMaterial(light_material);

		std::shared_ptr<Triangle> light_mesh0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 0, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) })
			);
		light_mesh0->setMaterialId(light_material_id);
		std::shared_ptr<AreaLight> light0 = std::make_shared<AreaLight>(glm::vec3(10, 10, 10), light_mesh0);
		scene.addLight(light0, light_mesh0);

		std::shared_ptr<Triangle> light_mesh1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 1, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) })
			);
		light_mesh1->setMaterialId(light_material_id);
		std::shared_ptr<AreaLight> light1 = std::make_shared<AreaLight>(glm::vec3(10, 10, 10), light_mesh1);
		scene.addLight(light1, light_mesh1);

		float aspect_ratio = static_cast<float>(ApplicationOld::WIDTH) / ApplicationOld::HEIGHT;
		glm::vec3 look_from = glm::vec3(0, 0, -6);
		glm::vec3 look_at = glm::vec3(0);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	std::shared_ptr<Camera> cerberus_scene(Scene &scene)
	{
		TransformParameters t;
		t.translation = glm::vec3(0, 0, 200);
		t.rotation_rads = glm::vec3(0, -M_PI / 12, 0);
		std::string model_path = "Cerberus_by_Andrew_Maximov/Cerberus_LP.fbx";
		if (!ModelLoader::loadModel(model_path, scene, Transform(t))) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}

		std::shared_ptr<PerfectSpecularMaterial> mirror_material = std::make_shared<PerfectSpecularMaterial>();
		int mirror_material_id = scene.addMaterial(mirror_material);

		std::shared_ptr<PerfectDiffuseMaterial> lambertian_debug_material = std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.1f)));
		int lambertian_debug_material_id = scene.addMaterial(lambertian_debug_material);

		std::shared_ptr<DielectricMaterial> fresnel_material = std::make_shared<DielectricMaterial>(2.56f);
		int fresnel_material_id = scene.addMaterial(fresnel_material);

		std::shared_ptr<MatteMaterial> matte_material = std::make_shared<MatteMaterial>(std::make_shared<ConstantTexture>(glm::vec3(1)));
		int matte_material_id = scene.addMaterial(matte_material);

		std::shared_ptr<GlossyMaterial> plastic_material = std::make_shared<GlossyMaterial>(1.5f, std::make_shared<ConstantTexture>(glm::vec3(0.9f, 0.4f, 0.8f)), 0.01f);
		int plastic_material_id = scene.addMaterial(plastic_material);

		scene.getShapes()[0]->setMaterialId(matte_material_id);

		//std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(0, 0, 0), 10.f);
		std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(0, 0, 100), 10.f);
		sphere->setMaterialId(plastic_material_id);
		scene.addShape(sphere);

		//std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(0, 0, 0), 10.f);
		std::shared_ptr<Sphere> sphere2 = std::make_shared<Sphere>(glm::vec3(0, 25, 100), 10.f);
		sphere2->setMaterialId(mirror_material_id);
		scene.addShape(sphere2);

		std::shared_ptr<EmissiveMaterial> light_material = std::make_shared<EmissiveMaterial>(std::make_shared<ConstantTexture>(glm::vec3(1.5, 1.5, 1.5)), ConstantTexture::black);
		int light_material_id = scene.addMaterial(light_material);

		TransformParameters light_transform;
		light_transform.translation = glm::vec3(-30, 0, 100);
		light_transform.scaling = glm::vec3(20, 20, 20);
		std::shared_ptr<Triangle> light_mesh0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(0, -0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(0, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(0, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) })
			);
		light_mesh0->transform(Transform(light_transform));
		light_mesh0->setMaterialId(light_material_id);
		std::shared_ptr<AreaLight> light0 = std::make_shared<AreaLight>(glm::vec3(10000, 10000, 10000), light_mesh0);
		scene.addLight(light0, light_mesh0);

		std::shared_ptr<Triangle> light_mesh1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(0, 0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1) }),
			Vertex({ glm::vec3(0, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(0, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) })
			);
		light_mesh1->transform(Transform(light_transform));
		light_mesh1->setMaterialId(light_material_id);
		std::shared_ptr<AreaLight> light1 = std::make_shared<AreaLight>(glm::vec3(10000, 10000, 10000), light_mesh1);
		scene.addLight(light1, light_mesh1);

		float aspect_ratio = static_cast<float>(ApplicationOld::WIDTH) / ApplicationOld::HEIGHT;
		//glm::vec3 look_from = glm::vec3(50, 0, 30);
		glm::vec3 look_from = glm::vec3(0, 0, -6);
		glm::vec3 look_at = glm::vec3(0);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	std::shared_ptr<Camera> cornell_box_scene(Scene& scene) {
		cornell_box(scene);

		float aspect_ratio = static_cast<float>(ApplicationOld::WIDTH) / ApplicationOld::HEIGHT;
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
	/*
	std::clock_t main_clock(std::clock());

    std::clock_t sub_clock(std::clock());
    
	std::cout << "Initializing window and OpenGL context...";
    ApplicationOld ApplicationOld;
    ApplicationOld.init();
    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;
    
    sub_clock = std::clock();
    std::cout << "Initializing ray tracer...";
    RayTracer ray_tracer;
    ray_tracer.init();
    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;

	sub_clock = std::clock();
	std::cout << "Loading scene data...";
	*/

	Scene* scene = nullptr;

	/*
	scene = SceneFactory::createScene();

	std::shared_ptr<Camera> camera =  cerberus_scene(*scene);

	std::shared_ptr<InfiniteAreaLight> environmentLight = std::make_shared<InfiniteAreaLight>("lakeside_2k.hdr");
	//std::shared_ptr<InfiniteAreaLight> environmentLight = std::make_shared<InfiniteAreaLight>("test_sun.hdr");
	//std::shared_ptr<InfiniteAreaLight> environmentLight = std::make_shared<InfiniteAreaLight>("test_white.hdr");
	scene->addLight(environmentLight);
	*/

	Application application;
	if (int result = application.loadScene("viking_room/viking_room.obj")) {
		return result;
	}

	if (int result = application.start()) {
		return result;
	}

	application.mainLoop();

	/*
	ray_tracer.setCamera(camera);
	ray_tracer.setScene(*scene);

    std::cout << " done (took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds)." << std::endl;

    sub_clock = std::clock();
    std::cout << "Started ray tracing:" << std::endl;

	scene->computeAccelerationStructures();
	ray_tracer.start();

    bool finished = false;
    do {
		finished = ray_tracer.iterate();
		finished |= !ApplicationOld.refreshWindow(ray_tracer.getImageBuffer());
    } while (!finished);
	
	std::fstream file_stream;
	file_stream.open("image_buffer.txt", std::ios::out);
	const float* image_buffer = ray_tracer.getImageBuffer();
	for (int y = 0; y < ApplicationOld::HEIGHT; ++y) {
		for (int x = 0; x < ApplicationOld::WIDTH * 3; x+=3) {
			file_stream << image_buffer[y * ApplicationOld::WIDTH * 3 + x] << " ";
			file_stream << image_buffer[y * ApplicationOld::WIDTH * 3 + x + 1] << " ";
			file_stream << image_buffer[y * ApplicationOld::WIDTH * 3 + x + 2] << std::endl;
		}
	}
	file_stream.close();

    std::cout << "Ray tracing took " << (double(std::clock()) - sub_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;
	
	std::cout << "End. Process took " << (double(std::clock()) - main_clock) / (double)CLOCKS_PER_SEC << " seconds." << std::endl;

    ApplicationOld.idle();
	*/

    return 0;
}

