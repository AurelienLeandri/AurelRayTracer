#define _USE_MATH_DEFINES
#include <math.h>

#include <fstream>
#include <iostream>

#include "Application.h"
#include "PathTracer.h"
#include "Scene.h"
#include "Light.h"
#include "PathTracer.h"
#include "Material.h"
#include "Sphere.h"
#include "Camera.h"
#include "Distribution2D.h"
#include "Triangle.h"
#include "ModelLoader.h"
#include "Mesh.h"

namespace {
	void material_test_scene(SceneData& scene, const std::vector<std::shared_ptr<Material>>& materials);
	int make_box(SceneData& scene, int material_id, const Transform& t);
	void cornell_box(SceneData& scene);
	std::shared_ptr<Camera> backpack_scene(SceneData& scene);
	std::shared_ptr<Camera> cerberus_scene(SceneData& scene);
	std::shared_ptr<Camera> cornell_box_scene(SceneData& scene);
}

int main() {
	std::cout << "Loading test scenes. This can take a few seconds." << std::endl;

	PathTracer::Parameters params;
	params.strategy = PathTracer::ImportanceSamplingStrategy::LightsAndBSDF;
	params.integratorStrategy = PathTracer::IntegratorStrategy::SimplePathTracer;
	params.nbSamples = 200;
	params.maxDepth = 50;
	params.shuffleRandom = true;

	std::vector<int> scenes = {
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7
	};

	scenes = {
		0,
	};

	std::vector<std::string> sceneNames = {
		"MatteMaterialSigmaStep10deg",
		"DielectricMaterialEta256RoughnessStep01",
		"DielectricMaterialEta1To2Step01Roughness02",
		"PerfectMaterials",
		"PlasticRoughnessStep01",
		"CornellBox",
		"Backpack",
		"Cerberus"
	};

	std::vector<std::vector<std::shared_ptr<Material>>> sceneMaterials;
	// Matte materials sigma
	sceneMaterials.push_back({});
	for (int i = 0; i < 10; ++i) {
		sceneMaterials.back().push_back(std::make_shared<MatteMaterial>(ConstantTexture::white, (i * 10.f / 180) * M_PI));
	}
	// Dielectric materials roughness
	sceneMaterials.push_back({});
	sceneMaterials.back().push_back(std::make_shared<DielectricMaterial>(2.56f, ConstantTexture::white, 0));
	for (int i = 0; i < 9; ++i) {
		sceneMaterials.back().push_back(std::make_shared<DielectricMaterial>(2.56f, ConstantTexture::white, (i + 1) * (1.f / 9)));
	}
	// Dielectric materials eta
	sceneMaterials.push_back({});
	sceneMaterials.back().push_back(std::make_shared<DielectricMaterial>(1.f, ConstantTexture::white, 0.f));
	for (int i = 0; i < 9; ++i) {
		sceneMaterials.back().push_back(std::make_shared<DielectricMaterial>(1.f + (i + 1) * (1.f / 9), ConstantTexture::white, 0.f));
	}
	// Perfect materials
	sceneMaterials.push_back({});
	sceneMaterials.back().push_back(std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.8f, 0.5f, 0.4f))));
	sceneMaterials.back().push_back(std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.8f, 0.5f, 0.4f))));
	sceneMaterials.back().push_back(std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.8f, 0.5f, 0.4f))));
	//sceneMaterials.back().push_back(std::make_shared<PerfectSpecularMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.3f, 0.8f, 0.4f))));
	//sceneMaterials.back().push_back(std::make_shared<PerfectTransparentMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.6f, 0.5f, 0.8f))));
	// Plastic materials roughness
	sceneMaterials.push_back({});
	sceneMaterials.back().push_back(std::make_shared<GlossyMaterial>(1.5f, std::make_shared<ConstantTexture>(glm::vec3(0.8f, 0.5f, 0.4f)), 0.f));
	for (int i = 0; i < 9; ++i) {
		sceneMaterials.back().push_back(std::make_shared<GlossyMaterial>(1.5f, std::make_shared<ConstantTexture>(glm::vec3(0.8f, 0.5f, 0.4f)), (i + 1) * (1.f / 90)));
	}

	sceneMaterials.push_back({});
	sceneMaterials.push_back({});
	sceneMaterials.push_back({});
	

	std::vector<std::shared_ptr<Camera>> cameras;
	float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
	glm::vec3 look_from = glm::vec3(0, 50, -35);
	look_from /= 1.5f;
	glm::vec3 look_at = glm::vec3(0, 0, 0);
	float dist_to_focus = 10.f;
	float aperture = 0.f;
	float fov = (80.f * float(M_PI)) / 180.f;
	cameras.push_back(std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f));
	look_from = glm::vec3(0, 0, -25);
	cameras.push_back(std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f));

	std::vector<SceneData*> modelScenes;
	for (int i = 0; i < 5; ++i) {
		modelScenes.push_back(nullptr);
	}
	for (int i = 5; i < 8; ++i) {
		modelScenes.push_back(SceneFactory::createScene());
	}
	cameras.push_back(cornell_box_scene(*modelScenes[5]));
	cameras.push_back(backpack_scene(*modelScenes[6]));
	cameras.push_back(cerberus_scene(*modelScenes[7]));

	std::vector<int> sceneCameras = {
		0, 0, 0, 1, 0, 2, 3, 4
	};

	std::vector<std::shared_ptr<InfiniteAreaLight>> environmentLights = {
		std::make_shared<InfiniteAreaLight>("resources/hdr/lakeside_2k.hdr"),
		std::make_shared<InfiniteAreaLight>("resources/hdr/test_sun.hdr"),
		std::make_shared<InfiniteAreaLight>("resources/hdr/test_white.hdr"),
		std::make_shared<InfiniteAreaLight>("resources/hdr/test_directions.hdr"),
	};
	environmentLights[3]->transform(Transform(glm::vec3(0), glm::vec3(0, M_PI, 0), glm::vec3(1)));
	std::vector<int> sceneEnvironments = {
		0, 0, 0, 1, 1, -1, 0, 0  // -1 is black
	};

	for (int i : scenes) {
		Application application;
		application.init();

		PathTracer ray_tracer(params);

		ray_tracer.init();
		SceneData* scene = nullptr;

		if (sceneMaterials[i].size()) {  // Material test scene
			scene = SceneFactory::createScene();
			material_test_scene(*scene, sceneMaterials[i]);
		}
		else {  // Model
			scene = modelScenes[i];
		}

		std::shared_ptr<Camera> camera = cameras[sceneCameras[i]];

		if (sceneEnvironments[i] >= 0) {
			scene->addLight(environmentLights[sceneEnvironments[i]]);
		}

		ray_tracer.setCamera(camera);
		ray_tracer.setScene(*scene);

		scene->computeAccelerationStructures();
		ray_tracer.start();

		bool finished = false;
		do {
			finished = ray_tracer.iterate();
			finished |= !application.refreshWindow(ray_tracer.getImageBuffer());
		} while (!finished);

		const float *image_buffer = ray_tracer.getImageBuffer();

		bool clampColors = true;

		std::ofstream os;
		os.open(sceneNames[i] + ".ppm");
		os << "P3" << std::endl;
		os << Application::WIDTH << " " << Application::HEIGHT << std::endl;
		os << "255" << std::endl;
		for (int i = 0; i < Application::HEIGHT; ++i) {
			for (int j = 0; j < Application::WIDTH * 3; j += 3) {
				float r = image_buffer[i * Application::WIDTH * 3 + j] * 255;
				float g = image_buffer[i * Application::WIDTH * 3 + j + 1] * 255;
				float b = image_buffer[i * Application::WIDTH * 3 + j + 2] * 255;
				if (clampColors) {
					os << int(glm::min(255.f, r)) << " ";
					os << int(glm::min(255.f, g)) << " ";
					os << int(glm::min(255.f, b)) << " ";
				}
				else {
					os << int(r) << " ";
					os << int(g) << " ";
					os << int(b) << " ";
				}
			}
			os << std::endl;
		}
		os.flush();
		os.close();
	}
	return 0;
}

namespace {
	void material_test_scene(SceneData& scene, const std::vector<std::shared_ptr<Material>>& materials) {
		int nbSpheres = materials.size();
		int x_offset = nbSpheres >= 5 ? -20 : -(nbSpheres / 2) * 10;
		for (int i = 0; i < nbSpheres; ++i) {
			std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(x_offset + (i % 5) * 10, 0, 5 - (i / 5) * 10), 5.f);
			int material_id = scene.addMaterial(materials[i]);
			sphere->materialId = material_id;
			scene.addShape(sphere);
		}

		/*
		std::shared_ptr<EmissiveMaterial> light_material = std::make_shared<EmissiveMaterial>(std::make_shared<ConstantTexture>(glm::vec3(1000, 1000, 1000)), ConstantTexture::black);
		int material_light_id = scene.addMaterial(light_material);

		// Light plane
		std::shared_ptr<Triangle> triangle_light_0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-10, 100, -10), glm::vec3(0, -1, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(10, 100, -10), glm::vec3(0, -1, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(-10, 100, 10), glm::vec3(0, -1, 0), glm::vec2(0, 1) })
			);
		std::shared_ptr<Triangle> triangle_light_1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(10, 100, 10), glm::vec3(0, -1, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(-10, 100, 10), glm::vec3(0, -1, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(10, 100, -10), glm::vec3(0, -1, 0), glm::vec2(1, 0) })
			);
		triangle_light_0->materialId = material_light_id;
		triangle_light_1->materialId = material_light_id;
		std::shared_ptr<AreaLight> light0 = std::make_shared<AreaLight>(glm::vec3(1000, 1000, 1000), triangle_light_0);
		scene.addLight(light0, triangle_light_0);
		std::shared_ptr<AreaLight> light1 = std::make_shared<AreaLight>(glm::vec3(1000, 1000, 1000), triangle_light_1);
		scene.addLight(light1, triangle_light_1);
		*/
	}

	int make_box(SceneData& scene, int material_id, const Transform& t = {})
	{
		std::shared_ptr<Mesh> box_mesh = std::make_shared<Mesh>();

		box_mesh->geometry.push_back({ glm::vec3(0), glm::vec3(0, 0, -1), glm::vec2(0, 0) });  // 0
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1) });  // 1
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0) });  // 2
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1) });  // 1
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 0), glm::vec3(0, 0, -1), glm::vec2(1, 1) });  // 3
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0) });  // 2

		box_mesh->geometry.push_back({ glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec2(0, 0) });  // 4
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) });  // 6
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(1), glm::vec3(0, 0, 1), glm::vec2(1, 1) });  // 7
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) });  // 6

		box_mesh->geometry.push_back({ glm::vec3(0), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });  // 0
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });  // 1
		box_mesh->geometry.push_back({ glm::vec3(0), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });  // 0
		box_mesh->geometry.push_back({ glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });  // 4
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });  // 5

		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec2(1, 0) });  // 2
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 1), glm::vec3(1, 0, 0), glm::vec2(1, 0) });  // 6
		box_mesh->geometry.push_back({ glm::vec3(1), glm::vec3(1, 0, 0), glm::vec2(1, 1) });  // 7
		box_mesh->geometry.push_back({ glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec2(1, 0) });  // 2
		box_mesh->geometry.push_back({ glm::vec3(1), glm::vec3(1, 0, 0), glm::vec2(1, 1) });  // 7
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 0), glm::vec3(1, 0, 0), glm::vec2(1, 1) });  // 3

		box_mesh->geometry.push_back({ glm::vec3(1, 1, 0), glm::vec3(0, 1, 0), glm::vec2(1, 1) });  // 3
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec2(0, 1) });  // 1
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(1, 1, 0), glm::vec3(0, 1, 0), glm::vec2(1, 1) });  // 3
		box_mesh->geometry.push_back({ glm::vec3(0, 1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1) });  // 5
		box_mesh->geometry.push_back({ glm::vec3(1), glm::vec3(0, 1, 0), glm::vec2(1, 1) });  // 7

		box_mesh->geometry.push_back({ glm::vec3(0), glm::vec3(0, -1, 0), glm::vec2(0, 0) });  // 0
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

		return scene.addShape(box_mesh);
	}

	void cornell_box(SceneData& scene) {
		std::shared_ptr<PerfectDiffuseMaterial> material_red = std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.65f, 0.05f, 0.05f)));
		std::shared_ptr<PerfectDiffuseMaterial> material_white = std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.73f, 0.73f, 0.73f)));
		std::shared_ptr<PerfectDiffuseMaterial> material_green = std::make_shared<PerfectDiffuseMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.12f, 0.45f, 0.15f)));
		std::shared_ptr<EmissiveMaterial> material_light = std::make_shared<EmissiveMaterial>(std::make_shared<ConstantTexture>(glm::vec3(15, 15, 15)), ConstantTexture::black);

		int material_red_id = scene.addMaterial(material_red);
		int material_green_id = scene.addMaterial(material_green);
		int material_white_id = scene.addMaterial(material_white);
		int material_light_id = scene.addMaterial(material_light);

		// Green plane
		std::shared_ptr<Mesh> green_plane = std::make_shared<Mesh>();
		green_plane->geometry.push_back({ glm::vec3(555, 0, 0), glm::vec3(1, 0, 0), glm::vec2(0, 0) });
		green_plane->geometry.push_back({ glm::vec3(555, 555, 0), glm::vec3(1, 0, 0), glm::vec2(0, 1) });
		green_plane->geometry.push_back({ glm::vec3(555, 0, 555), glm::vec3(1, 0, 0), glm::vec2(1, 0) });
		green_plane->geometry.push_back({ glm::vec3(555, 555, 555), glm::vec3(1, 0, 0), glm::vec2(1, 1) });
		green_plane->indices = { 0, 1, 2, 3, 2, 1 };
		green_plane->materialId = material_green_id;
		scene.addShape(green_plane);

		// Red plane
		std::shared_ptr<Mesh> red_plane = std::make_shared<Mesh>();
		red_plane->geometry.push_back({ glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });
		red_plane->geometry.push_back({ glm::vec3(0, 0, 555), glm::vec3(-1, 0, 0), glm::vec2(1, 0) });
		red_plane->geometry.push_back({ glm::vec3(0, 555, 0), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });
		red_plane->geometry.push_back({ glm::vec3(0, 555, 555), glm::vec3(-1, 0, 0), glm::vec2(1, 1) });
		red_plane->indices = { 0, 1, 2, 3, 2, 1 };
		red_plane->materialId = material_red_id;
		scene.addShape(red_plane);

		// White ceiling
		std::shared_ptr<Mesh> white_ceiling = std::make_shared<Mesh>();
		white_ceiling->geometry.push_back({ glm::vec3(0, 555, 0), glm::vec3(0, -1, 0), glm::vec2(0, 0) });
		white_ceiling->geometry.push_back({ glm::vec3(555, 555, 0), glm::vec3(0, -1, 0), glm::vec2(1, 0) });
		white_ceiling->geometry.push_back({ glm::vec3(0, 555, 555), glm::vec3(0, -1, 0), glm::vec2(0, 1) });
		white_ceiling->geometry.push_back({ glm::vec3(555, 555, 555), glm::vec3(0, -1, 0), glm::vec2(0, 0) });
		white_ceiling->indices = { 0, 1, 2, 3, 2, 1 };
		white_ceiling->materialId = material_white_id;
		scene.addShape(white_ceiling);


		// White plane in the back
		std::shared_ptr<Mesh> white_plane = std::make_shared<Mesh>();
		white_plane->geometry.push_back({ glm::vec3(0, 0, 555), glm::vec3(0, 0, -1), glm::vec2(0, 0) });
		white_plane->geometry.push_back({ glm::vec3(0, 555, 555), glm::vec3(0, 0, -1), glm::vec2(1, 0) });
		white_plane->geometry.push_back({ glm::vec3(555, 0, 555), glm::vec3(0, 0, -1), glm::vec2(0, 1) });
		white_plane->geometry.push_back({ glm::vec3(555, 555, 555), glm::vec3(0, 0, -1), glm::vec2(0, 0) });
		white_plane->indices = { 0, 1, 2, 3, 2, 1 };
		white_plane->materialId = material_white_id;
		scene.addShape(white_plane);

		// White floor
		std::shared_ptr<Mesh> white_floor = std::make_shared<Mesh>();
		white_floor->geometry.push_back({ glm::vec3(0), glm::vec3(0, 1, 0), glm::vec2(0, 0) });
		white_floor->geometry.push_back({ glm::vec3(0, 0, 555), glm::vec3(0, 1, 0), glm::vec2(0, 1) });
		white_floor->geometry.push_back({ glm::vec3(555, 0, 0), glm::vec3(0, 1, 0), glm::vec2(1, 0) });
		white_floor->geometry.push_back({ glm::vec3(555, 0, 555), glm::vec3(0, 1, 0), glm::vec2(0, 0) });
		white_floor->indices = { 0, 1, 2, 3, 2, 1 };
		white_floor->materialId = material_white_id;
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
		triangle_light_0->materialId = material_light_id;
		triangle_light_1->materialId = material_light_id;
		std::shared_ptr<AreaLight> light0 = std::make_shared<AreaLight>(glm::vec3(15, 15, 15), triangle_light_0);
		scene.addLight(light0, triangle_light_0);
		std::shared_ptr<AreaLight> light1 = std::make_shared<AreaLight>(glm::vec3(15, 15, 15), triangle_light_1);
		scene.addLight(light1, triangle_light_1);

		// Small box
		TransformParameters cubic_box_transform;
		cubic_box_transform.scaling = glm::vec3(165);
		cubic_box_transform.translation = glm::vec3(130, 0, 65);
		cubic_box_transform.rotation_rads = glm::vec3(0, -18 * (float(M_PI) / 180.f), 0);
		make_box(scene, material_white_id, Transform(cubic_box_transform));

		// Big box
		TransformParameters tall_box_transform;
		tall_box_transform.scaling = glm::vec3(165, 330, 165);
		tall_box_transform.translation = glm::vec3(265, 0, 295);
		tall_box_transform.rotation_rads = glm::vec3(0, 15 * (float(M_PI) / 180.f), 0);
		make_box(scene, material_white_id, Transform(tall_box_transform));
	}

	std::shared_ptr<Camera> backpack_scene(SceneData& scene)
	{
		TransformParameters t;
		t.translation = glm::vec3(0.5f, 0, 3);
		t.scaling = glm::vec3(1);
		t.rotation_rads = glm::vec3(0, -(3 * M_PI) / 4, 0);
		std::string model_path = "resources/models/Survival_BackPack_2/backpack.obj";
		if (!ModelLoader::loadModel(model_path, scene, Transform(t))) {
			std::cerr << "Could not load model " << model_path << std::endl;
		}
		scene.getShapes()[0]->materialId = 1;

		std::shared_ptr<EmissiveMaterial> light_material = std::make_shared<EmissiveMaterial>(std::make_shared<ConstantTexture>(glm::vec3(100, 100, 100)), ConstantTexture::black);
		int light_material_id = scene.addMaterial(light_material);

		std::shared_ptr<Triangle> light_mesh0 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 0, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) })
			);
		light_mesh0->materialId = light_material_id;
		std::shared_ptr<AreaLight> light0 = std::make_shared<AreaLight>(glm::vec3(100, 100, 100), light_mesh0);
		scene.addLight(light0, light_mesh0);

		std::shared_ptr<Triangle> light_mesh1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(-3, 1, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1) }),
			Vertex({ glm::vec3(-3, 0, 3.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(-3, 1, 2.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) })
			);
		light_mesh1->materialId = light_material_id;
		std::shared_ptr<AreaLight> light1 = std::make_shared<AreaLight>(glm::vec3(100, 100, 100), light_mesh1);
		scene.addLight(light1, light_mesh1);

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		glm::vec3 look_from = glm::vec3(0, 0, -6);
		glm::vec3 look_at = glm::vec3(0);
		float dist_to_focus = 10.f;
		float aperture = 0.f;
		float fov = (40.f * float(M_PI)) / 180.f;
		return std::make_shared<Camera>(look_from, look_at, glm::vec3(0.f, 1.f, 0.f), fov, aspect_ratio, aperture, dist_to_focus, 0.f, 1.f);
	}

	std::shared_ptr<Camera> cerberus_scene(SceneData& scene)
	{
		TransformParameters t;
		t.translation = glm::vec3(0, 0, 200);
		t.rotation_rads = glm::vec3(0, -M_PI / 12, 0);
		std::string model_path = "resources/models/Cerberus_by_Andrew_Maximov/Cerberus_LP.fbx";
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

		//scene.getShapes()[0]->materialId = matte_material_id;
		scene.getShapes()[0]->materialId = 1;

		//std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(0, 0, 0), 10.f);
		std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(0, 0, 100), 10.f);
		sphere->materialId = plastic_material_id;
		scene.addShape(sphere);

		//std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(0, 0, 0), 10.f);
		/*
		std::shared_ptr<Sphere> sphere2 = std::make_shared<Sphere>(glm::vec3(0, 25, 100), 10.f);
		sphere2->materialId = mirror_material_id;
		scene.addShape(sphere2);
		*/

		std::shared_ptr<EmissiveMaterial> light_material = std::make_shared<EmissiveMaterial>(std::make_shared<ConstantTexture>(glm::vec3(1000, 1000, 1000)), ConstantTexture::black);
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
		light_mesh0->materialId = light_material_id;
		std::shared_ptr<AreaLight> light0 = std::make_shared<AreaLight>(glm::vec3(1000, 1000, 1000), light_mesh0);
		scene.addLight(light0, light_mesh0);

		std::shared_ptr<Triangle> light_mesh1 = std::make_shared<Triangle>(
			Vertex({ glm::vec3(0, 0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1) }),
			Vertex({ glm::vec3(0, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0) }),
			Vertex({ glm::vec3(0, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1) })
			);
		light_mesh1->transform(Transform(light_transform));
		light_mesh1->materialId = light_material_id;
		std::shared_ptr<AreaLight> light1 = std::make_shared<AreaLight>(glm::vec3(1000, 1000, 1000), light_mesh1);
		scene.addLight(light1, light_mesh1);

		float aspect_ratio = static_cast<float>(Application::WIDTH) / Application::HEIGHT;
		//glm::vec3 look_from = glm::vec3(50, 0, 30);
		glm::vec3 look_from = glm::vec3(0, 0, -6);
		glm::vec3 look_at = glm::vec3(0);
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
