#include "NavigationRenderer.h"

#include <exception>
#include <iostream>
#include <array>

// TODO: remove if possible once readFile is refactored/deleted
#include <fstream>

namespace {
    // TODO: remove/make better
    std::vector<char> readFile(const std::string& filename);
}

NavigationRenderer::NavigationRenderer(GLFWwindow& window, VulkanInstance& vulkan) :
	Renderer(window, vulkan)
{
}

NavigationRenderer::~NavigationRenderer()
{
}

int NavigationRenderer::init()
{
    if (!Renderer::init()) {
        try {
            _depthBufferFormat = _findDepthFormat();
            _createRenderPass();
            _createDescriptorSetLayout();
            _createGraphicsPipeline();
            _createColorResources();
            _createDepthResources();
            _createFramebuffers();
            _createUniformBuffers();
            // TODO
            // Create Vertex and index buffer
            // Create descriptor sets without knowing about the scene beforehand?
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

// TODO: create a load scene function
// Fill buffers

int NavigationRenderer::cleanup()
{
	return 0;
}

int NavigationRenderer::iterate()
{
    try {
        _drawFrame();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}

void NavigationRenderer::_createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _vulkanProperties.swapChainImageFormat;
    colorAttachment.samples = _vulkanProperties.maxNbMsaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = _depthBufferFormat;
    depthAttachment.samples = _vulkanProperties.maxNbMsaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = _vulkanProperties.swapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = {
        colorAttachment,
        depthAttachment,
        colorAttachmentResolve
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

void NavigationRenderer::_createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(_vulkan.getLogicalDevice(), &layoutInfo, nullptr, &_descriptorSetLayout)) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
}

void NavigationRenderer::_createGraphicsPipeline() {
    // TODO: compile and check shaders here or somewhere in the code, to get rid of
    // having to run compile.bat every single time

    // Shaders
    std::vector<char> vertShaderCode = readFile("Shaders/vert.spv");
    std::vector<char> fragShaderCode = readFile("Shaders/frag.spv");

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
    multisampling.rasterizationSamples = _vulkanProperties.maxNbMsaaSamples;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
    // Antialiazing on shading, for example textures with sudden color changes.
    // Has a performance cost.
    multisampling.sampleShadingEnable = VK_TRUE;
    multisampling.minSampleShading = .2f; // min fraction for sample shading; closer to one is smooth

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
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
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
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

VkShaderModule NavigationRenderer::_createShaderModule(const std::vector<char>& code) {
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

void NavigationRenderer::_createCommandPool() {
    const VulkanInstance::QueueFamilyIndices& queueFamilyIndices = _vulkan.getQueueFamilyIndices();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = 0;

    if (vkCreateCommandPool(_vulkan.getLogicalDevice(), &poolInfo, nullptr, &_commandPool)) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void NavigationRenderer::_createColorResources() {
    VkFormat colorFormat = _vulkanProperties.swapChainImageFormat;

    _createImage(_vulkanProperties.swapChainExtent.width, _vulkanProperties.swapChainExtent.height, 1, _vulkanProperties.maxNbMsaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _colorImage, _colorImageMemory);
    _colorImageView = _createImageView(_colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void NavigationRenderer::_createDepthResources() {
    _createImage(_vulkanProperties.swapChainExtent.width, _vulkanProperties.swapChainExtent.height,
        1, _vulkanProperties.maxNbMsaaSamples, _depthBufferFormat, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        _depthImage, _depthImageMemory);
    _depthImageView = _createImageView(_depthImage, _depthBufferFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

void NavigationRenderer::_createFramebuffers() {
    const std::vector<VkImageView>& swapChainImageViews = _vulkan.getSwapChainImageViews();

    _swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<VkImageView, 3> attachments = {
            _colorImageView,
            _depthImageView,
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

void NavigationRenderer::_createUniformBuffers() {
    const std::vector<VkImageView>& swapChainImageViews = _vulkan.getSwapChainImageViews();

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    _uniformBuffers.resize(swapChainImageViews.size());
    _uniformBuffersMemory.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        _createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            _uniformBuffers[i], _uniformBuffersMemory[i]);
    }
}

void NavigationRenderer::_createCommandBuffers() {
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

        // TODO
        /*
        VkBuffer vertexBuffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

        vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        */

        vkCmdEndRenderPass(_commandBuffers[i]);

        if (vkEndCommandBuffer(_commandBuffers[i])) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

void NavigationRenderer::_createSyncObjects() {
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

void NavigationRenderer::_drawFrame() {
    VkDevice& device = _vulkan.getLogicalDevice();
    vkWaitForFences(device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        device, _vulkan.getSwapChain(), UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    // Swap chain is invalid or suboptimal (for example because of a window resize)
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        _vulkan.recreateSwapChain();
        _recreateSwapChainDependentResources();
        return;
    }
    else if (result && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // TODO:
    //_updateUniformBuffer(imageIndex);

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
        _vulkan.recreateSwapChain();
        _recreateSwapChainDependentResources();
    }
    else if (result) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    _currentFrame = (_currentFrame + 1) % _MAX_FRAMES_IN_FLIGHT;
}

void NavigationRenderer::_recreateSwapChainDependentResources() {
    _createRenderPass();
    _createGraphicsPipeline();
    _createColorResources();
    _createDepthResources();
    _createFramebuffers();
    _createUniformBuffers();
    // TODO
    //_createDescriptorPool();
    //_createDescriptorSets();
    _createCommandBuffers();
}

VkFormat NavigationRenderer::_findDepthFormat() const {
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
