// Copyright (c) 2022.
// License: CC0 1.0 Universal
// Permissions:
// - Commercial use
// - Modification
// - Distribution
// - Private use
//
// Limitations:
// - Liability
// - Trademark use
// - Patent use
// - Warranty
//
// Author: Danil Andreev | danssg08@gmail.com | https://github.com/DanilAndreev

#include "SPIRVShaders.h"
#include "tools/CMTVulkanBackend.h"
#include <cstdint>
#include <glm/glm.hpp>

namespace ShaderDataStructs {
    using namespace glm;
    using uint = std::uint32_t;

    struct MorphingSettings {
        uint strokeElementsCount;
    };
}// namespace ShaderDataStructs

VkResult CMTVulkanBackend::initialize() noexcept {
    VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.pApplicationName = "curve-morphing-tool";
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);

    const char *instanceExtensionNames[] = {
#ifdef IMT_DEBUG
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    };

    //TODO: select from synced list
    const char *instanceLayerNames[] = {
#ifdef IMT_DEBUG
            "VK_LAYER_RENDERDOC_Capture",
            "VK_LAYER_KHRONOS_validation"
#endif
    };


    VkInstanceCreateInfo instanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledLayerCount = sizeof(instanceLayerNames) / sizeof(instanceLayerNames[0]);
    instanceCreateInfo.ppEnabledLayerNames = instanceLayerNames;
    instanceCreateInfo.enabledExtensionCount = sizeof(instanceExtensionNames) / sizeof(instanceExtensionNames[0]);
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensionNames;

    VkResult status;
    status = vkCreateInstance(&instanceCreateInfo, this->allocator, &this->instance);
    if (status != VK_SUCCESS) return status;

    uint32_t queueFamilyIndex;
    status = this->selectPhysicalDevice(&queueFamilyIndex);
    if (status != VK_SUCCESS) return status;

    VkDeviceQueueCreateInfo deviceQueueCreateInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = nullptr;

    VkDeviceCreateInfo deviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};

    VkPhysicalDeviceFeatures enabledDeviceFeatures{};

    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = &enabledDeviceFeatures;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

    status = vkCreateDevice(this->physicalDevice, &deviceCreateInfo, this->allocator, &this->device);
    if (status != VK_SUCCESS) return status;

    vkGetDeviceQueue(this->device, deviceQueueCreateInfo.queueFamilyIndex, 0, &this->deviceQueue);

    status = this->initializeShaderModules();
    if (status != VK_SUCCESS) return status;

    VkCommandPoolCreateInfo commandPoolCreateInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
    status = vkCreateCommandPool(this->device, &commandPoolCreateInfo, this->allocator, &this->commandPool);
    if (status != VK_SUCCESS) return status;

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    commandBufferAllocateInfo.commandPool = this->commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;
    status = vkAllocateCommandBuffers(this->device, &commandBufferAllocateInfo, &this->commandBuffer);
    if (status != VK_SUCCESS) return status;

    std::vector<VkDescriptorSetLayout> setLayouts;
    status = this->createPipelineLayout(&this->pipelineLayout, &setLayouts);
    if (status != VK_SUCCESS) return status;

    VkDescriptorPoolSize poolDescriptorTypeSizes[] = {
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.poolSizeCount = sizeof(poolDescriptorTypeSizes) / sizeof(poolDescriptorTypeSizes[0]);
    descriptorPoolCreateInfo.pPoolSizes = poolDescriptorTypeSizes;
    status = vkCreateDescriptorPool(this->device, &descriptorPoolCreateInfo, this->allocator, &this->descriptorPool);
    if (status != VK_SUCCESS) return status;

    VkDescriptorSetAllocateInfo setAllocateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    setAllocateInfo.descriptorPool = this->descriptorPool;
    setAllocateInfo.descriptorSetCount = setLayouts.size();
    setAllocateInfo.pSetLayouts = setLayouts.data();

    this->descriptorSets.resize(setAllocateInfo.descriptorSetCount);
    status = vkAllocateDescriptorSets(this->device, &setAllocateInfo, this->descriptorSets.data());
    if (status != VK_SUCCESS) return status;

    for (const VkDescriptorSetLayout &setLayout: setLayouts) {
        vkDestroyDescriptorSetLayout(this->device, setLayout, this->allocator);
    }

    return VK_SUCCESS;
}

void CMTVulkanBackend::release() noexcept {
    vkFreeCommandBuffers(this->device, this->commandPool, 1, &this->commandBuffer);
    this->commandBuffer = VK_NULL_HANDLE;
    vkDestroyCommandPool(this->device, this->commandPool, this->allocator);
    this->commandPool = VK_NULL_HANDLE;
    this->releaseShaderModules();
    vkDestroyDevice(this->device, this->allocator);
    this->device = VK_NULL_HANDLE;
    this->physicalDevice = VK_NULL_HANDLE;
    vkDestroyInstance(this->instance, this->allocator);
    this->instance = VK_NULL_HANDLE;
}

VkResult CMTVulkanBackend::selectPhysicalDevice(uint32_t *outQueueFamilyIndex) noexcept {
    VkResult status;
    std::uint32_t count;
    std::vector<VkPhysicalDevice> physicalDevices{};
    status = vkEnumeratePhysicalDevices(this->instance, &count, nullptr);
    if (status != VK_SUCCESS) return status;

    physicalDevices.resize(count);
    status = vkEnumeratePhysicalDevices(this->instance, &count, physicalDevices.data());
    if (status != VK_SUCCESS) return status;

    if (physicalDevices.empty()) return VK_ERROR_INITIALIZATION_FAILED;

    for (const VkPhysicalDevice &physDevice: physicalDevices) {
        std::vector<VkQueueFamilyProperties> properties{};
        uint32_t propertiesCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &propertiesCount, nullptr);
        properties.resize(propertiesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &propertiesCount, properties.data());
        for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < properties.size(); ++queueFamilyIndex) {
            //TODO: more complex logic;
            this->physicalDevice = physDevice;
            *outQueueFamilyIndex = queueFamilyIndex;
            return VK_SUCCESS;
        }
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

static VkResult createShaderModule(VkDevice device,
                                   VkAllocationCallbacks *allocator,
                                   const uint32_t *SPIRVBinary,
                                   std::size_t size,
                                   VkShaderModule *outShaderModule) noexcept {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = size;
    createInfo.pCode = SPIRVBinary;
    return vkCreateShaderModule(device, &createInfo, allocator, outShaderModule);
}

VkResult CMTVulkanBackend::initializeShaderModules() noexcept {
    using ShaderBin_t = const uint32_t *;
    VkResult status;
    status = createShaderModule(this->device, allocator,
                                reinterpret_cast<ShaderBin_t>(image_morphing_tool_vert),
                                sizeof(image_morphing_tool_vert), &this->shaders.vertexShader);
    if (status != VK_SUCCESS) return status;
    status = createShaderModule(this->device, allocator,
                                reinterpret_cast<ShaderBin_t>(image_morphing_tool_frag),
                                sizeof(image_morphing_tool_frag), &this->shaders.fragmentShader);
    return status;
}
void CMTVulkanBackend::releaseShaderModules() noexcept {
    vkDestroyShaderModule(this->device, this->shaders.fragmentShader, allocator);
    this->shaders.fragmentShader = VK_NULL_HANDLE;
    vkDestroyShaderModule(this->device, this->shaders.vertexShader, allocator);
    this->shaders.vertexShader = VK_NULL_HANDLE;
}
VkResult CMTVulkanBackend::initializeRenderer() noexcept {
    std::vector<VkAttachmentDescription> attachments{};
    VkAttachmentDescription attachment{};
    attachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
    attachments.push_back(attachment);

    std::vector<VkSubpassDescription> subpasses{};
    std::vector<VkAttachmentReference> attachmentReferences = {
            {0, VK_IMAGE_LAYOUT_GENERAL},
    };

    VkSubpassDescription subpassDescription{};
    subpassDescription.flags = 0;
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.colorAttachmentCount = attachmentReferences.size();
    subpassDescription.pColorAttachments = attachmentReferences.data();
    subpassDescription.pResolveAttachments = nullptr;
    subpassDescription.pDepthStencilAttachment = nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;
    subpasses.push_back(subpassDescription);

    std::vector<VkSubpassDependency> dependencies{};

    VkRenderPassCreateInfo renderPassCreateInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    renderPassCreateInfo.attachmentCount = attachments.size();
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = subpasses.size();
    renderPassCreateInfo.pSubpasses = subpasses.data();
    renderPassCreateInfo.dependencyCount = dependencies.size();
    renderPassCreateInfo.pDependencies = dependencies.data();

    VkResult status;
    status = vkCreateRenderPass(this->device, &renderPassCreateInfo, this->allocator, &this->renderPass);
    if (status != VK_SUCCESS) return status;


    return VK_SUCCESS;
}

void CMTVulkanBackend::releaseRenderer() noexcept {
    vkDestroyRenderPass(this->device, this->renderPass, this->allocator);
    this->renderPass = VK_NULL_HANDLE;
}
VkResult CMTVulkanBackend::createPipelineLayout(VkPipelineLayout *outPipelineLayout,
                                                std::vector<VkDescriptorSetLayout> *outSetLayouts) noexcept {
    VkResult status;
    VkDescriptorSetLayoutBinding strokeFromSSBOLayoutBinding{};
    strokeFromSSBOLayoutBinding.binding = 0;
    strokeFromSSBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    strokeFromSSBOLayoutBinding.descriptorCount = 1;
    strokeFromSSBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    strokeFromSSBOLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding strokeToSSBOLayoutBinding{};
    strokeToSSBOLayoutBinding.binding = 1;
    strokeToSSBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    strokeToSSBOLayoutBinding.descriptorCount = 1;
    strokeToSSBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    strokeToSSBOLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding targetImageTextureLayoutBinding{};
    strokeToSSBOLayoutBinding.binding = 2;
    strokeToSSBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    strokeToSSBOLayoutBinding.descriptorCount = 1;
    strokeToSSBOLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    strokeToSSBOLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding bindings[] = {strokeFromSSBOLayoutBinding,
                                               strokeToSSBOLayoutBinding,
                                               targetImageTextureLayoutBinding};
    VkDescriptorSetLayoutCreateInfo createInfoUniform{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    createInfoUniform.bindingCount = sizeof(bindings) / sizeof(bindings[0]);
    createInfoUniform.pBindings = bindings;

    outSetLayouts->clear();
    outSetLayouts->resize(1);//TODO: add set layouts global config.
    status = vkCreateDescriptorSetLayout(this->device, &createInfoUniform, this->allocator, &outSetLayouts->at(0));
    if (status != VK_SUCCESS) return status;

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ShaderDataStructs::MorphingSettings);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.setLayoutCount = outSetLayouts->size();
    pipelineLayoutCreateInfo.pSetLayouts = outSetLayouts->data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    status = vkCreatePipelineLayout(this->device, &pipelineLayoutCreateInfo, this->allocator, outPipelineLayout);
    return status;
}

VkResult CMTVulkanBackend::execute(Image &image, const Stroke &fromStroke, const Stroke &toStroke) noexcept {
    this->allocateResources(image, fromStroke, toStroke);

    VkCommandBufferBeginInfo commandBufferBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    vkBeginCommandBuffer(this->commandBuffer, &commandBufferBeginInfo);

    vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);

    uint32_t dOffset = 0;
    vkCmdBindDescriptorSets(this->commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            this->pipelineLayout,
                            0,
                            this->descriptorSets.size(),
                            this->descriptorSets.data(),
                            1,
                            &dOffset);//TODO: caution

    ShaderDataStructs::MorphingSettings morphingSettings{};
    morphingSettings.strokeElementsCount = fromStroke.size();

    vkCmdPushConstants(this->commandBuffer, this->pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShaderDataStructs::MorphingSettings),
                       &morphingSettings);

    VkDeviceSize vertexBufferOffset = 0;
    //    vkCmdBindVertexBuffers(this->commandBuffer, 0, 1, &this->cubeVertexBuffer, &vertexBufferOffset);

    this->releaseResources();

    return VK_SUCCESS;
}

static void fillStrokeData(const Stroke &stroke, const Image &canvas, glm::vec2 *outData) {
    auto canvasWidth = static_cast<float>(canvas.width());
    auto canvasHeight = static_cast<float>(canvas.height());
    for (std::size_t i = 0; i < stroke.size(); ++i) {
        float x = static_cast<float>(stroke[i].x()) / canvasWidth;
        float y = static_cast<float>(stroke[i].y()) / canvasHeight;
        outData[i] = {x, y};
    }
}

VkResult CMTVulkanBackend::allocateResources(const Image &image, const Stroke &strokeFrom, const Stroke &strokeTo) noexcept {
    VkBufferCreateInfo strokeSSBOCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    strokeSSBOCreateInfo.size = strokeFrom.size() * sizeof(std::uint32_t);
    strokeSSBOCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    strokeSSBOCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    strokeSSBOCreateInfo.queueFamilyIndexCount = 0;
    strokeSSBOCreateInfo.pQueueFamilyIndices = nullptr;

    VkResult status;
    status = vkCreateBuffer(this->device, &strokeSSBOCreateInfo,
                            this->allocator, &this->fromStrokeBuffer);
    if (status != VK_SUCCESS) return status;
    strokeSSBOCreateInfo.size = strokeTo.size() * sizeof(std::uint32_t);
    status = vkCreateBuffer(this->device, &strokeSSBOCreateInfo,
                            this->allocator, &this->toStrokeBuffer);
    if (status != VK_SUCCESS) return status;

    VkMemoryRequirements strokeFromSSBOMemReq{};
    vkGetBufferMemoryRequirements(this->device, this->fromStrokeBuffer, &strokeFromSSBOMemReq);
    VkMemoryRequirements strokeToSSBOMemReq{};
    vkGetBufferMemoryRequirements(this->device, this->toStrokeBuffer, &strokeToSSBOMemReq);


    VkMemoryAllocateInfo strokeSSBOMemoryCreateInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    strokeSSBOMemoryCreateInfo.allocationSize = strokeFromSSBOMemReq.size;
    strokeSSBOMemoryCreateInfo.memoryTypeIndex = this->findMemoryType(strokeFromSSBOMemReq,
                                                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    status = vkAllocateMemory(this->device, &strokeSSBOMemoryCreateInfo,
                              this->allocator, &this->fromStrokeMemory);
    if (status != VK_SUCCESS) return status;

    strokeSSBOMemoryCreateInfo.allocationSize = strokeToSSBOMemReq.size;
    strokeSSBOMemoryCreateInfo.memoryTypeIndex = this->findMemoryType(strokeToSSBOMemReq,
                                                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    status = vkAllocateMemory(this->device, &strokeSSBOMemoryCreateInfo,
                              this->allocator, &this->toStrokeMemory);
    if (status != VK_SUCCESS) return status;

    void *mappedData = nullptr;
    status = vkMapMemory(this->device, this->fromStrokeMemory, 0, VK_WHOLE_SIZE, 0, &mappedData);
    if (status != VK_SUCCESS) return status;
    fillStrokeData(strokeFrom, image, reinterpret_cast<glm::vec2 *>(mappedData));
    vkUnmapMemory(this->device, this->fromStrokeMemory);

    status = vkMapMemory(this->device, this->toStrokeMemory, 0, VK_WHOLE_SIZE, 0, &mappedData);
    if (status != VK_SUCCESS) return status;
    fillStrokeData(strokeTo, image, reinterpret_cast<glm::vec2 *>(mappedData));
    vkUnmapMemory(this->device, this->toStrokeMemory);

    vkBindBufferMemory(this->device, this->fromStrokeBuffer, this->fromStrokeMemory, 0);
    vkBindBufferMemory(this->device, this->toStrokeBuffer, this->toStrokeMemory, 0);

    // -------------------------------------------------------------------------------------------------------------- TEXTURES
    VkImageCreateInfo textureCreateInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    textureCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    textureCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    textureCreateInfo.extent = {static_cast<std::uint32_t>(image.width()),
                                static_cast<std::uint32_t>(image.height()), 1};
    textureCreateInfo.mipLevels = 1;
    textureCreateInfo.arrayLayers = 1;
    textureCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    textureCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    textureCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    textureCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    textureCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    status = vkCreateImage(this->device, &textureCreateInfo,
                           this->allocator, &this->sourceImage);
    if (status != VK_SUCCESS) return status;

    textureCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    textureCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    status = vkCreateImage(this->device, &textureCreateInfo,
                           this->allocator, &this->resultImage);
    if (status != VK_SUCCESS) return status;

    VkMemoryRequirements sourceTextureMemReq{};
    vkGetImageMemoryRequirements(this->device, this->sourceImage, &sourceTextureMemReq);
    VkMemoryRequirements resultTextureMemReq{};
    vkGetImageMemoryRequirements(this->device, this->sourceImage, &resultTextureMemReq);

    VkMemoryAllocateInfo textureMemoryCreateInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    textureMemoryCreateInfo.allocationSize = sourceTextureMemReq.size;
    textureMemoryCreateInfo.memoryTypeIndex = this->findMemoryType(sourceTextureMemReq,
                                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    status = vkAllocateMemory(this->device, &textureMemoryCreateInfo,
                              this->allocator, &this->sourceImageMemory);
    if (status != VK_SUCCESS) return status;

    textureMemoryCreateInfo.allocationSize = resultTextureMemReq.size;
    textureMemoryCreateInfo.memoryTypeIndex = this->findMemoryType(resultTextureMemReq,
                                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    status = vkAllocateMemory(this->device, &textureMemoryCreateInfo,
                              this->allocator, &this->resultImageMemory);
    if (status != VK_SUCCESS) return status;

    vkBindImageMemory(this->device, this->sourceImage, this->sourceImageMemory, 0);
    vkBindImageMemory(this->device, this->resultImage, this->resultImageMemory, 0);

    VkImageViewCreateInfo depthViewCreateInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    depthViewCreateInfo.image = this->sourceImage;
    depthViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthViewCreateInfo.format = textureCreateInfo.format;
    depthViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    depthViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    depthViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    depthViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    depthViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    depthViewCreateInfo.subresourceRange.baseMipLevel = 0;
    depthViewCreateInfo.subresourceRange.levelCount = 1;
    depthViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    depthViewCreateInfo.subresourceRange.layerCount = 1;

    status = vkCreateImageView(this->device, &depthViewCreateInfo,
                               this->allocator, &this->sourceImageView);
    if (status != VK_SUCCESS) return status;

    depthViewCreateInfo.image = this->resultImage;
    status = vkCreateImageView(this->device, &depthViewCreateInfo,
                               this->allocator, &this->resultImageView);
    if (status != VK_SUCCESS) return status;

    // -------------------------------------------------------------------------------------------------------------- LOAD/READBACK BUFFER
    VkBufferCreateInfo loadReadBufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    loadReadBufferCreateInfo.size = sourceTextureMemReq.size;
    loadReadBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    loadReadBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    loadReadBufferCreateInfo.queueFamilyIndexCount = 0;
    loadReadBufferCreateInfo.pQueueFamilyIndices = nullptr;
    status = vkCreateBuffer(this->device, &loadReadBufferCreateInfo,
                            this->allocator, &this->loadReadBuffer);
    if (status != VK_SUCCESS) return status;
    VkMemoryRequirements loadReadBufferMemReq{};
    vkGetBufferMemoryRequirements(this->device, this->loadReadBuffer, &loadReadBufferMemReq);


    VkMemoryAllocateInfo loadReadBufferMemoryCreateInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    loadReadBufferMemoryCreateInfo.allocationSize = loadReadBufferMemReq.size;
    loadReadBufferMemoryCreateInfo.memoryTypeIndex = this->findMemoryType(strokeFromSSBOMemReq,
                                                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    status = vkAllocateMemory(this->device, &loadReadBufferMemoryCreateInfo,
                              this->allocator, &this->loadReadBufferMemory);
    if (status != VK_SUCCESS) return status;

//    status = vkMapMemory(this->device, this->sourceImageMemory, 0, VK_WHOLE_SIZE, 0, &mappedData);
//    if (status != VK_SUCCESS) return status;
//    const unsigned char *imageData = image.constBits();
//    memcpy(mappedData, imageData, image.sizeInBytes());//todo: careful
//    vkUnmapMemory(this->device, this->toStrokeMemory);


    return VK_SUCCESS;
}

void CMTVulkanBackend::releaseResources() noexcept {
    vkDestroyImageView(this->device, this->resultImageView, this->allocator);
    this->resultImageView = VK_NULL_HANDLE;
    vkDestroyImageView(this->device, this->sourceImageView, this->allocator);
    this->sourceImageView = VK_NULL_HANDLE;
    vkDestroyImage(this->device, this->resultImage, this->allocator);
    this->resultImage = VK_NULL_HANDLE;
    vkDestroyImage(this->device, this->sourceImage, this->allocator);
    this->sourceImage = VK_NULL_HANDLE;
    vkFreeMemory(this->device, this->sourceImageMemory, this->allocator);
    this->sourceImageMemory = VK_NULL_HANDLE;
    vkFreeMemory(this->device, this->resultImageMemory, this->allocator);
    this->resultImageMemory = VK_NULL_HANDLE;

    vkDestroyBuffer(this->device, this->fromStrokeBuffer, this->allocator);
    this->fromStrokeBuffer = VK_NULL_HANDLE;
    vkDestroyBuffer(this->device, this->toStrokeBuffer, this->allocator);
    this->toStrokeBuffer = VK_NULL_HANDLE;
    vkFreeMemory(this->device, this->fromStrokeMemory, this->allocator);
    this->fromStrokeMemory = VK_NULL_HANDLE;
    vkFreeMemory(this->device, this->toStrokeMemory, this->allocator);
    this->toStrokeMemory = VK_NULL_HANDLE;
}

std::uint32_t CMTVulkanBackend::findMemoryType(const VkMemoryRequirements &memoryRequirements,
                                               VkMemoryPropertyFlags requiredFlags,
                                               VkMemoryPropertyFlags preferredFlags) noexcept {
    preferredFlags |= requiredFlags;
    VkPhysicalDeviceMemoryProperties memoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &memoryProperties);
    uint32_t selectedType = ~0u;
    uint32_t memoryType;
    for (memoryType = 0; memoryType < 32; ++memoryType) {
        if (memoryRequirements.memoryTypeBits & (1 << memoryType)) {
            const VkMemoryType &type = memoryProperties.memoryTypes[memoryType];
            if ((type.propertyFlags & preferredFlags) == preferredFlags) {
                selectedType = memoryType;
                break;
            }
        }
    }

    if (selectedType != ~0u) {
        for (memoryType = 0; memoryType < 32; ++memoryType) {
            if (memoryRequirements.memoryTypeBits & (1 << memoryType)) {
                const VkMemoryType &type = memoryProperties.memoryTypes[memoryType];
                if ((type.propertyFlags & requiredFlags) == requiredFlags) {
                    selectedType = memoryType;
                    break;
                }
            }
        }
    }

    return selectedType;
}
