#pragma once

#include "Renderer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class NavigationRenderer :
    public Renderer
{
public:
	NavigationRenderer(GLFWwindow& window, VulkanInstance& vulkan);
	virtual ~NavigationRenderer() override;

public:
	virtual int init() override;
	virtual int cleanup() override;

	virtual int iterate();

private:
	void _createRenderPass();
	VkFormat _findDepthFormat() const;
	void _createDescriptorSetLayout();
	void _createGraphicsPipeline();
	VkShaderModule _createShaderModule(const std::vector<char>& code);
	void _createCommandPool();
	void _createColorResources();
	void _createDepthResources();
	void _createFramebuffers();
	void _createUniformBuffers();
	void _createCommandBuffers();
	void _createSyncObjects();
	void _recreateSwapChainDependentResources();

	void _drawFrame();

private:
	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

private:
	VkFormat _depthBufferFormat = VK_FORMAT_UNDEFINED;
	VkDescriptorSetLayout _descriptorSetLayout;
	VkPipelineLayout _pipelineLayout;
	VkPipeline _graphicsPipeline;
	VkCommandPool _commandPool;
	VkImage _colorImage;
	VkDeviceMemory _colorImageMemory;
	VkImageView _colorImageView;
	VkImage _depthImage;
	VkDeviceMemory _depthImageMemory;
	VkImageView _depthImageView;
	std::vector<VkFramebuffer> _swapChainFramebuffers;
	std::vector<VkBuffer> _uniformBuffers;
	std::vector<VkDeviceMemory> _uniformBuffersMemory;
	std::vector<VkCommandBuffer> _commandBuffers;
	std::vector<VkSemaphore> _imageAvailableSemaphores;
	std::vector<VkSemaphore> _renderFinishedSemaphores;
	const int _MAX_FRAMES_IN_FLIGHT = 2;
	std::vector<VkFence> _inFlightFences;
	std::vector<VkFence> _imagesInFlight;
	size_t _currentFrame = 0;
};

