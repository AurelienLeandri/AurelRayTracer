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
	void _createColorResources();
	void _createDepthResources();
	void _createFramebuffers();
	void _createVertexBuffers();
	void _createIndexBuffer();
	void _createUniformBuffers();
	void _createCommandBuffers();
	void _createSyncObjects();
	void _cleanupSwapChainDependentResources();
	void _recreateSwapChainDependentResources();
	void _createTextureImage();
	void _createTextureImageView();
	void _createTextureSampler();
	void _transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void _copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void _generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	void _createDescriptorPool();
	void _createDescriptorSets();
	void _updateUniformBuffer(uint32_t currentImage);
	void _createCommandPool();
	void _copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void _drawFrame();

private:
	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

private:
	/*
	TODO: put as much renderer-agnostic data in VulkanInstance, and data common to all renderer
	in Renderer
	*/
	VkFormat _depthBufferFormat = VK_FORMAT_UNDEFINED;
	VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
	VkPipeline _graphicsPipeline = VK_NULL_HANDLE;
	VkImage _colorImage = VK_NULL_HANDLE;
	VkDeviceMemory _colorImageMemory = VK_NULL_HANDLE;
	VkImageView _colorImageView = VK_NULL_HANDLE;
	VkImage _depthImage = VK_NULL_HANDLE;
	VkDeviceMemory _depthImageMemory = VK_NULL_HANDLE;
	VkImageView _depthImageView = VK_NULL_HANDLE;
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
	VkBuffer _vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory _vertexBufferMemory = VK_NULL_HANDLE;
	VkBuffer _indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory _indexBufferMemory = VK_NULL_HANDLE;
	VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> _descriptorSets;
	VkRenderPass _renderPass = VK_NULL_HANDLE;
	VkCommandPool _commandPool = VK_NULL_HANDLE;

	// TODO: Should be removed
	VkImage _textureImage = VK_NULL_HANDLE;
	VkDeviceMemory _textureImageMemory = VK_NULL_HANDLE;
	VkImageView _textureImageView = VK_NULL_HANDLE;
	VkSampler _textureSampler = VK_NULL_HANDLE;
	unsigned int _mipLevels = 0;
};

