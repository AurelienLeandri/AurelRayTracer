#include "WhittedIntegrator.h"

#include "HitRecord.h"
#include "Scene.h"
#include "Light.h"

WhittedIntegrator::WhittedIntegrator(int maxDepth, std::shared_ptr<Camera> camera, std::shared_ptr<Sampler> sampler)
	: SamplerIntegrator(sampler, camera), _maxDepth(maxDepth)
{
}

glm::vec3 WhittedIntegrator::Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, int depth) const
{
	glm::vec3 radiance(0);
	HitRecord record;
	if (!scene.castRay(ray, record)) {
		for (std::shared_ptr<const Light> light : scene.getLights()) {
			radiance += light->radianceInDirection(ray.direction);
		}
		return radiance;
	}

	glm::vec3 normal = record.normal;
	glm::vec3 wo = glm::normalize(-ray.direction);
	if (record.areaLight)
		radiance += record.areaLight->radianceInDirection(-wo);

	for (std::shared_ptr<const Light> light : scene.getLights()) {
        float light_sample_proba = 0;
        glm::vec3 light_sample(0, 0, 1);
        glm::vec3 L = light->sampleLi(light_sample, record, light_sample_proba);

        // Shadow ray visibility test
        Ray direct_lighting_ray(record.position, light_sample);
        light_sample = glm::normalize(light_sample);
        HitRecord occlusion_hit_record;
        if (light->getType() == LightType::AREA) {
            // If the light can be sampled from our position, we check if we hit the light:
            // To verify this, "occlusion_hit_record.tRay" should be very close to one since "light_sample" stretches from the current position to the light.
            if (light_sample_proba > 0.000001f && scene.castRay(direct_lighting_ray, occlusion_hit_record) && occlusion_hit_record.tRay >= 0.9999f) {
                float cos_light_surface = glm::abs(glm::dot(light_sample, record.normal));
                glm::vec3 light_f = record.bsdf.f(light_sample, wo, record);
                radiance += L * light_f * cos_light_surface / light_sample_proba;
            }
        }
	}

	if (depth + 1 < _maxDepth) {
		radiance += specularReflect(ray, record, scene, sampler, depth);
		radiance += specularTransmit(ray, record, scene, sampler, depth);
	}

	return radiance;
}
