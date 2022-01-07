#define _USE_MATH_DEFINES
#include <math.h>

#include <fstream>

#include "Application.h"
#include "RayTracer.h"
#include "Scene.h"
#include "Light.h"
#include "RayTracer.h"
#include "Material.h"
#include "Sphere.h"
#include "Camera.h"
#include "Distribution2D.h"


namespace {
	void material_test_scene(SceneData& scene, const std::vector<std::shared_ptr<Material>>& materials) {
		int nbSpheres = materials.size();
		int x_offset = nbSpheres >= 5 ? -20 : -(nbSpheres / 2) * 10;
		for (int i = 0; i < nbSpheres; ++i) {
			std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(glm::vec3(x_offset + (i % 5) * 10, 0, 5 - (i / 5) * 10), 5);
			int material_id = scene.addMaterial(materials[i]);
			sphere->materialId = material_id;
			scene.addShape(sphere);
		}
	}
}

int main() {
	std::vector<int> scenes = {
		0,
		1,
		2,
		3,
		4,
	};

	std::vector<int> nbSamples = {
		256,
		256,
		256,
		256,
		256
	};

	std::vector<std::string> sceneNames = {
		"MatteMaterialSigmaStep10deg",
		"DielectricMaterialEta256RoughnessStep01",
		"DielectricMaterialEta1To2Step01Roughness02",
		"PerfectMaterials",
		"PlasticRoughnessStep01",
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
	sceneMaterials.back().push_back(std::make_shared<PerfectSpecularMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.3f, 0.8f, 0.4f))));
	sceneMaterials.back().push_back(std::make_shared<PerfectTransparentMaterial>(std::make_shared<ConstantTexture>(glm::vec3(0.6f, 0.5f, 0.8f))));
	// Plastic materials roughness
	sceneMaterials.push_back({});
	sceneMaterials.back().push_back(std::make_shared<GlossyMaterial>(1.5f, std::make_shared<ConstantTexture>(glm::vec3(0.8f, 0.5f, 0.4f)), 0.f));
	for (int i = 0; i < 9; ++i) {
		sceneMaterials.back().push_back(std::make_shared<GlossyMaterial>(1.5f, std::make_shared<ConstantTexture>(glm::vec3(0.8f, 0.5f, 0.4f)), (i + 1) * (1.f / 90)));
	}

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

	std::vector<int> sceneCameras = {
		0, 0, 0, 1, 0
	};

	std::vector<std::shared_ptr<InfiniteAreaLight>> environmentLights = {
		std::make_shared<InfiniteAreaLight>("lakeside_2k.hdr"),
		std::make_shared<InfiniteAreaLight>("test_sun.hdr"),
		std::make_shared<InfiniteAreaLight>("test_white.hdr"),
		std::make_shared<InfiniteAreaLight>("test_directions.hdr"),
	};
	environmentLights[3]->transform(Transform(glm::vec3(0), glm::vec3(0, M_PI, 0), glm::vec3(1)));
	std::vector<int> sceneEnvironments = {
		0, 0, 0, 0, 1
	};

	for (int i : scenes) {
		Application application;
		application.init();

		RayTracer::Parameters params;
		params.nbSamples = nbSamples[i];
		RayTracer ray_tracer(params);

		ray_tracer.init();
		SceneData* scene = SceneFactory::createScene();

		material_test_scene(*scene, sceneMaterials[i]);

		std::shared_ptr<Camera> camera = cameras[sceneCameras[i]];

		scene->addLight(environmentLights[sceneEnvironments[i]]);

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
		os.open("RenderTests/" + sceneNames[i] + ".ppm");
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
