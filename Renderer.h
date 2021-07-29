#pragma once

#include "VulkanInstance.h"

class Renderer
{
public:
	Renderer(GLFWwindow& window, VulkanInstance& vulkan);
	virtual ~Renderer() = default;

public:
	virtual int init();
	virtual int cleanup();

	virtual int iterate() = 0;

protected:
	void _createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView _createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void _createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	// TODO: make protected once ApplicationOld can be cut off
public:
	static VkVertexInputBindingDescription _getVertexBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> _getVertexAttributeDescriptions();

protected:
	GLFWwindow& _window;;
	VulkanInstance& _vulkan;
	const VulkanInstance::Properties& _vulkanProperties;
	VkRenderPass _renderPass = VK_NULL_HANDLE;
};

