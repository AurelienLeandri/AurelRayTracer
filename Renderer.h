#pragma once

#include "VulkanInstance.h"

class Scene;
class Camera;

class Renderer
{
public:
	Renderer(GLFWwindow& window, VulkanInstance& vulkan);
	virtual ~Renderer() = default;

public:
	virtual int init();
	virtual int start() { return 0; }  // TODO
	virtual int stop() { return 0; }  // TODO
	virtual int cleanup();
	virtual void setCamera(const Camera& camera);

	virtual int iterate() = 0;

	void setCamera(const Camera* camera);
	void setScene(const Scene* scene);

protected:
	void _createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView _createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void _createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void _copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	VkCommandBuffer _beginSingleTimeCommands(VkCommandPool& commandPool);
	void _endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool& commandPool);

	// TODO: make protected once ApplicationOld can be cut off
public:
	static VkVertexInputBindingDescription _getVertexBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> _getVertexAttributeDescriptions();

protected:
	GLFWwindow& _window;;
	VulkanInstance& _vulkan;
	const VulkanInstance::Properties& _vulkanProperties;
	const Camera* _camera = nullptr;
	const Scene* _scene = nullptr;
};

