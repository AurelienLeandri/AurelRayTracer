#include "RayTracerCPU.h"

#include <exception>
#include <iostream>
#include <array>

// TODO: remove if possible once readFile is refactored/deleted
#include <fstream>

#include "Scene.h"
#include "Transform.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "Camera.h"
#include "PathTracer.h"

// TODO: remove and use texture class
#include <stb_image.h>

namespace {
    // TODO: remove/make better
    std::vector<char> readFile(const std::string& filename);
}

RayTracerCPU::RayTracerCPU(GLFWwindow& window, VulkanInstance& vulkan) :
	Renderer(window, vulkan)
{
}

RayTracerCPU::~RayTracerCPU()
{
}

int RayTracerCPU::init()
{
    if (!Renderer::init()) {
        try {
            _createRenderPass();
            _createDescriptorSetLayout();
            _createGraphicsPipeline();
            _createCommandPool();
            _createFramebuffers();
            // TODO
            // Create descriptor sets without knowing about the scene beforehand, because we always expect the same set of textures and data?

            _createVertexBuffer();
            _createIndexBuffer();

            _createIntegrator();
            _integrator->setCamera(_camera);
            _integrator->setScene(_scene);

            _createTextureImage();
            _createTextureImageView();
            _createTextureSampler();

            _createDescriptorPool();
            _createDescriptorSets();

            _createCommandBuffers();
            _createSyncObjects();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
        return 0;
    }
    return -1;
}

int RayTracerCPU::cleanup()
{
    VkDevice& device = _vulkan.getLogicalDevice();

    vkDeviceWaitIdle(device);

    _cleanupSwapChainDependentResources();

    vkDestroySampler(device, _textureSampler, nullptr);
    _textureSampler = VK_NULL_HANDLE;
    vkDestroyImageView(device, _textureImageView, nullptr);
    _textureImageView = VK_NULL_HANDLE;

    vkDestroyImage(device, _textureImage, nullptr);
    _textureImage = VK_NULL_HANDLE;
    vkFreeMemory(device, _textureImageMemory, nullptr);
    _textureImageMemory = VK_NULL_HANDLE;

    vkDestroyBuffer(device, _stagingBuffer, nullptr);
    _stagingBuffer = VK_NULL_HANDLE;
    vkFreeMemory(device, _stagingBufferMemory, nullptr);
    _stagingBufferMemory = VK_NULL_HANDLE;

    vkDestroyDescriptorSetLayout(device, _descriptorSetLayout, nullptr);
    _descriptorSetLayout = VK_NULL_HANDLE;

    vkDestroyBuffer(device, _indexBuffer, nullptr);
    _indexBuffer = VK_NULL_HANDLE;
    vkFreeMemory(device, _indexBufferMemory, nullptr);
    _indexBufferMemory = VK_NULL_HANDLE;

    vkDestroyBuffer(device, _vertexBuffer, nullptr);
    _vertexBuffer = VK_NULL_HANDLE;
    vkFreeMemory(device, _vertexBufferMemory, nullptr);
    _vertexBufferMemory = VK_NULL_HANDLE;

    _imagesInFlight.clear();
    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, _renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, _imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, _inFlightFences[i], nullptr);
    }
    _renderFinishedSemaphores.clear();
    _imageAvailableSemaphores.clear();
    _inFlightFences.clear();

    vkDestroyCommandPool(device, _commandPool, nullptr);
    _commandPool = VK_NULL_HANDLE;

	return 0;
}

int RayTracerCPU::iterate()
{
    try {
        if (!_integrator->iterate()) {
            _updateTextureImage();
        }
        _drawFrame();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}

void RayTracerCPU::_createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _vulkanProperties.swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = nullptr;
    subpass.pResolveAttachments = nullptr;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 1> attachments = {
        colorAttachment,
    };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(_vulkan.getLogicalDevice(), &renderPassInfo, nullptr, &_renderPass)) {
        throw std::runtime_error("Failed to create render pass");
    }
}

void RayTracerCPU::_createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = { samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(_vulkan.getLogicalDevice(), &layoutInfo, nullptr, &_descriptorSetLayout)) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
}

void RayTracerCPU::_createGraphicsPipeline() {
    // TODO: compile and check shaders here or somewhere in the code, to get rid of
    // having to run compile.bat every single time

    // Shaders
    std::vector<char> vertShaderCode = readFile("Shaders/CPURTvert.spv");
    std::vector<char> fragShaderCode = readFile("Shaders/CPURTfrag.spv");

    VkShaderModule vertShaderModule = _createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = _createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // Fixed-function stages
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    auto bindingDescription = _getVertexBindingDescription();
    auto attributeDescriptions = _getVertexAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)_vulkanProperties.swapChainExtent.width;
    viewport.height = (float)_vulkanProperties.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = _vulkanProperties.swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
    // Antialiazing on shading, for example textures with sudden color changes.
    // Has a performance cost.
    multisampling.sampleShadingEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkDynamicState dynamicStates[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(_vulkan.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout)) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    VkDevice& device = _vulkan.getLogicalDevice();
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline)) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    fragShaderModule = VK_NULL_HANDLE;
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    vertShaderModule = VK_NULL_HANDLE;
}

VkShaderModule RayTracerCPU::_createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(_vulkan.getLogicalDevice(), &createInfo, nullptr, &shaderModule)) {
        throw std::runtime_error("Failed to create shader module");
    }

    return shaderModule;
}

void RayTracerCPU::_createFramebuffers() {
    const std::vector<VkImageView>& swapChainImageViews = _vulkan.getSwapChainImageViews();

    _swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<VkImageView, 1> attachments = {
            swapChainImageViews[i],
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = _vulkanProperties.swapChainExtent.width;
        framebufferInfo.height = _vulkanProperties.swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(_vulkan.getLogicalDevice(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i])) {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}

void RayTracerCPU::_createCommandBuffers() {
    _commandBuffers.resize(_swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size();

    VkDevice& device = _vulkan.getLogicalDevice();

    if (vkAllocateCommandBuffers(device, &allocInfo, _commandBuffers.data())) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < _commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo)) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _renderPass;
        renderPassInfo.framebuffer = _swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _vulkanProperties.swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

        VkBuffer vertexBuffers[] = { _vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(_commandBuffers[i], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(_commandBuffers[i], _indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
            _pipelineLayout, 0, 1, &_descriptorSets[i], 0, nullptr);

        vkCmdDrawIndexed(_commandBuffers[i], 6, 1, 0, 0, 0);

        vkCmdEndRenderPass(_commandBuffers[i]);

        if (vkEndCommandBuffer(_commandBuffers[i])) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

void RayTracerCPU::_createTextureImage() {
    const std::vector<unsigned char>& image = _integrator->getImageBuffer();
    uint32_t width = static_cast<uint32_t>(_integrator->getImageWidth());
    uint32_t height = static_cast<uint32_t>(_integrator->getImageHeight());
    uint32_t nbChannels = static_cast<uint32_t>(_integrator->getNbChannels());
    uint32_t imageSize = width * height * nbChannels;

    _createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _stagingBuffer, _stagingBufferMemory);

    VkDevice &device = _vulkan.getLogicalDevice();
    
    void* data;
    vkMapMemory(device, _stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, image.data(), static_cast<size_t>(imageSize));
    vkUnmapMemory(device, _stagingBufferMemory);

    _createImage(width, height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _textureImage, _textureImageMemory);

    _transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
    _copyBufferToImage(_stagingBuffer, _textureImage, width, height);

    _transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
}


void RayTracerCPU::_transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
    VkCommandBuffer commandBuffer = _beginSingleTimeCommands(_commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    _endSingleTimeCommands(commandBuffer, _commandPool);
}

void RayTracerCPU::_copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    // TODO: Try to setup a command buffer to record several operations and then flush everything once, instead of calling beginSingleTimeCommands and endSingleTimeCommands each time. Could also be done for uniforms and other stuff that creates single-time command buffers several times.
    VkCommandBuffer commandBuffer = _beginSingleTimeCommands(_commandPool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    _endSingleTimeCommands(commandBuffer, _commandPool);
}

void RayTracerCPU::_createTextureImageView() {
    _textureImageView = _createImageView(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void RayTracerCPU::_createTextureSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;

    samplerInfo.maxAnisotropy = _vulkanProperties.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 1.f;
    samplerInfo.mipLodBias = 0.0f;

    VkDevice& device = _vulkan.getLogicalDevice();
    if (vkCreateSampler(device, &samplerInfo, nullptr, &_textureSampler)) {
        throw std::runtime_error("Failed to create texture sampler");
    }
}

void RayTracerCPU::_createVertexBuffer()
{
    static const std::vector<Vertex> vertices = {
    {{-1.0f, -1.0f, 0.f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
    {{1.0f, -1.0f, 0.f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f, 0.f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
    {{-1.0f, 1.0f, 0.f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}
    };

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    _createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    VkDevice device = _vulkan.getLogicalDevice();
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    _createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

    _copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    stagingBuffer = VK_NULL_HANDLE;
    vkFreeMemory(device, stagingBufferMemory, nullptr);
    stagingBufferMemory = VK_NULL_HANDLE;
}

void RayTracerCPU::_createIndexBuffer()
{
    static const std::vector<unsigned int> indices = {
    0, 1, 2, 2, 3, 0
    };
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    _createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    VkDevice& device = _vulkan.getLogicalDevice();
    void* data = nullptr;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    _createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

    _copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    stagingBuffer = VK_NULL_HANDLE;
    vkFreeMemory(device, stagingBufferMemory, nullptr);
    stagingBufferMemory = VK_NULL_HANDLE;
}

void RayTracerCPU::_createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 1> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(_vulkan.getSwapChainSize());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(_vulkan.getSwapChainSize());

    VkDevice& device = _vulkan.getLogicalDevice();
    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &_descriptorPool)) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

void RayTracerCPU::_createDescriptorSets() {
    size_t swapChainSize = _vulkan.getSwapChainSize();
    std::vector<VkDescriptorSetLayout> layouts(swapChainSize, _descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainSize);
    allocInfo.pSetLayouts = layouts.data();

    _descriptorSets.resize(swapChainSize);
    VkDevice& device = _vulkan.getLogicalDevice();
    if (vkAllocateDescriptorSets(device, &allocInfo, _descriptorSets.data())) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < swapChainSize; i++) {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = _textureImageView;
        imageInfo.sampler = _textureSampler;

        std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(), 0, nullptr);
    }
}

void RayTracerCPU::_createSyncObjects() {
    const std::vector<VkImageView>& swapChainImageViews = _vulkan.getSwapChainImageViews();

    _imageAvailableSemaphores.resize(_MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(_MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(_MAX_FRAMES_IN_FLIGHT);
    _imagesInFlight.resize(swapChainImageViews.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkDevice& device = _vulkan.getLogicalDevice();

    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) ||
            vkCreateFence(device, &fenceInfo, nullptr, &_inFlightFences[i]))
        {
            throw std::runtime_error("Failed to create synchronization objects for a frame");
        }
    }
}

void RayTracerCPU::_drawFrame() {
    VkDevice& device = _vulkan.getLogicalDevice();
    vkWaitForFences(device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        device, _vulkan.getSwapChain(), UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    // Swap chain is invalid or suboptimal (for example because of a window resize)
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        _cleanupSwapChainDependentResources();
        _vulkan.recreateSwapChain();
        _recreateSwapChainDependentResources();
        return;
    }
    else if (result && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    _imagesInFlight[imageIndex] = _inFlightFences[_currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];
    VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &_inFlightFences[_currentFrame]);

    if (vkQueueSubmit(_vulkan.getGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _vulkan.getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(_vulkan.getPresentationQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || InputManager::framebufferResized) {
        InputManager::framebufferResized = false;
        _cleanupSwapChainDependentResources();
        _vulkan.recreateSwapChain();
        _recreateSwapChainDependentResources();
    }
    else if (result) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    _currentFrame = (_currentFrame + 1) % _MAX_FRAMES_IN_FLIGHT;
}

void RayTracerCPU::_createIntegrator()
{
    PathTracer::Parameters parameters = {};
    parameters.width = 600;
    parameters.height = 600;
    parameters.nbChannels = 4;
    _integrator = std::make_unique<PathTracer>(parameters);
    _integrator->init();
}

void RayTracerCPU::_updateTextureImage()
{
    const std::vector<unsigned char>& image = _integrator->getImageBuffer();
    uint32_t width = static_cast<uint32_t>(_integrator->getImageWidth());
    uint32_t height = static_cast<uint32_t>(_integrator->getImageHeight());
    uint32_t nbChannels = static_cast<uint32_t>(_integrator->getNbChannels());

    VkDevice device = _vulkan.getLogicalDevice();
    void* data = nullptr;
    uint32_t dataSize = width * height * nbChannels;

    vkMapMemory(device, _stagingBufferMemory, 0, dataSize, 0, &data);
    memcpy(data, image.data(), static_cast<size_t>(dataSize));
    vkUnmapMemory(device, _stagingBufferMemory);

    _transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
    _copyBufferToImage(_stagingBuffer, _textureImage, width, height);

    _transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
}

void RayTracerCPU::_recreateSwapChainDependentResources() {
    _createRenderPass();
    _createGraphicsPipeline();
    _createFramebuffers();
    _createDescriptorPool();
    _createDescriptorSets();
    _createCommandBuffers();
}

void RayTracerCPU::_cleanupSwapChainDependentResources()
{
    VkDevice& device = _vulkan.getLogicalDevice();

    vkDeviceWaitIdle(device);

    vkDestroyImageView(device, _colorImageView, nullptr);
    _colorImageView = VK_NULL_HANDLE;
    vkDestroyImage(device, _colorImage, nullptr);
    _colorImage = VK_NULL_HANDLE;
    vkFreeMemory(device, _colorImageMemory, nullptr);
    _colorImageMemory = VK_NULL_HANDLE;

    for (size_t i = 0; i < _swapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(device, _swapChainFramebuffers[i], nullptr);
    }
    _swapChainFramebuffers.clear();

    vkFreeCommandBuffers(device, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
    _commandBuffers.clear();

    vkDestroyPipeline(device, _graphicsPipeline, nullptr);
    _graphicsPipeline = VK_NULL_HANDLE;
    vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
    _pipelineLayout = VK_NULL_HANDLE;
    vkDestroyRenderPass(device, _renderPass, nullptr);
    _renderPass = VK_NULL_HANDLE;

    for (size_t i = 0; i < _uniformBuffers.size(); i++) {
        vkDestroyBuffer(device, _uniformBuffers[i], nullptr);
        vkFreeMemory(device, _uniformBuffersMemory[i], nullptr);
    }
    _uniformBuffers.clear();
    _uniformBuffersMemory.clear();

    vkDestroyDescriptorPool(device, _descriptorPool, nullptr);
    _descriptorPool = VK_NULL_HANDLE;
    _descriptorSets.clear();
}

void RayTracerCPU::_createCommandPool() {
    const VulkanInstance::QueueFamilyIndices& queueFamilyIndices = _vulkan.getQueueFamilyIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = 0;

    if (vkCreateCommandPool(_vulkan.getLogicalDevice(), &poolInfo, nullptr, &_commandPool)) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void RayTracerCPU::_copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = _beginSingleTimeCommands(_commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    _endSingleTimeCommands(commandBuffer, _commandPool);
}

VkFormat RayTracerCPU::_findDepthFormat() const {
    return _vulkan.findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

namespace {
    std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
}
