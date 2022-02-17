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
#include <renderdoc_app.h>

#include <libloaderapi.h>

#define ARR_ELEM_COUNT(arr) sizeof(arr) / sizeof(arr[0])

namespace ShaderDataStructs {
    using namespace glm;
    using uint = std::uint32_t;

    struct MorphingSettings {
        uint strokeElementsCount;
    };

    struct VertexData {
        vec2 vPosition;
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

    status = this->selectPhysicalDevice(&this->queueIndex);
    if (status != VK_SUCCESS) return status;

    VkDeviceQueueCreateInfo deviceQueueCreateInfo{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    deviceQueueCreateInfo.queueFamilyIndex = this->queueIndex;
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
    commandPoolCreateInfo.queueFamilyIndex = this->queueIndex;
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
    RENDERDOC_API_1_1_2 *rdoc_api = NULL;

    if (HMODULE mod = GetModuleHandleA("renderdoc.dll")) {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI =
                (pRENDERDOC_GetAPI) GetProcAddress(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **) &rdoc_api);
        assert(ret == 1);
    }

    if (rdoc_api) rdoc_api->StartFrameCapture(NULL, NULL);

    VkResult status;
    status = this->allocateResources(image, fromStroke, toStroke);
    if (status != VK_SUCCESS) return status;
    status = this->initializeRenderer();
    if (status != VK_SUCCESS) return status;

    VkFramebufferCreateInfo framebufferCreateInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferCreateInfo.renderPass = this->renderPass;
    framebufferCreateInfo.width = image.width();
    framebufferCreateInfo.height = image.height();
    framebufferCreateInfo.layers = 1;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = &this->resultImageView;
    status = vkCreateFramebuffer(this->device, &framebufferCreateInfo,
                                 this->allocator, &this->framebuffer);
    if (status != VK_SUCCESS) return status;

    status = this->createPSO(image);
    if (status != VK_SUCCESS) return status;

    VkCommandBufferBeginInfo commandBufferBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    status = vkBeginCommandBuffer(this->commandBuffer, &commandBufferBeginInfo);
    if (status != VK_SUCCESS) return status;
    vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);

    status = this->uploadResources(image);
    if (status != VK_SUCCESS) return status;

    uint32_t dOffset = 0;
    vkCmdBindDescriptorSets(this->commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            this->pipelineLayout,
                            0,
                            this->descriptorSets.size(),
                            this->descriptorSets.data(),
                            1,
                            &dOffset);//TODO: caution

    this->bindResources();

    ShaderDataStructs::MorphingSettings morphingSettings{};
    morphingSettings.strokeElementsCount = fromStroke.size();
    vkCmdPushConstants(this->commandBuffer, this->pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ShaderDataStructs::MorphingSettings),
                       &morphingSettings);


    VkClearValue colorClearValue{};
    colorClearValue.color = {};
    colorClearValue.color.float32[0] = 0.0f;
    colorClearValue.color.float32[1] = 0.0f;
    colorClearValue.color.float32[2] = 0.0f;
    colorClearValue.color.float32[3] = 0.0f;

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = this->renderPass;
    renderPassBeginInfo.framebuffer = this->framebuffer;
    renderPassBeginInfo.renderArea = {0, 0,
                                      static_cast<uint32_t>(image.width()),
                                      static_cast<uint32_t>(image.height())};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &colorClearValue;

    vkCmdBeginRenderPass(this->commandBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

    VkDeviceSize vertexBufferOffset = 0;
    vkCmdBindVertexBuffers(this->commandBuffer, 0, 1,
                           &this->vertexBuffer, &vertexBufferOffset);

    vkCmdDraw(this->commandBuffer, 6, 1, 0, 0);

    vkCmdEndRenderPass(this->commandBuffer);
    this->readbackPrepareResources(image);
    vkEndCommandBuffer(this->commandBuffer);

    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    vkQueueSubmit(this->deviceQueue, 1, &submitInfo, VK_NULL_HANDLE);

    vkQueueWaitIdle(this->deviceQueue);//TODO: semaphore

    this->readbackResources(image);
    this->releaseResources();

    if (rdoc_api) rdoc_api->EndFrameCapture(NULL, NULL);
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

    status = vkBindBufferMemory(this->device, this->fromStrokeBuffer, this->fromStrokeMemory, 0);
    if (status != VK_SUCCESS) return status;
    status = vkBindBufferMemory(this->device, this->toStrokeBuffer, this->toStrokeMemory, 0);
    if (status != VK_SUCCESS) return status;

    this->setDebugName(this->fromStrokeBuffer, "fromStrokeBuffer");
    this->setDebugName(this->toStrokeBuffer, "toStrokeBuffer");

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

    textureCreateInfo.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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

    status = vkBindImageMemory(this->device, this->sourceImage, this->sourceImageMemory, 0);
    if (status != VK_SUCCESS) return status;
    status = vkBindImageMemory(this->device, this->resultImage, this->resultImageMemory, 0);
    if (status != VK_SUCCESS) return status;


    this->setDebugName(this->sourceImage, "sourceImage");
    this->setDebugName(this->resultImage, "resultImage");

    VkImageViewCreateInfo resultImageViewCreateInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    resultImageViewCreateInfo.image = this->sourceImage;
    resultImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    resultImageViewCreateInfo.format = textureCreateInfo.format;
    resultImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    resultImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    resultImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    resultImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    resultImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    resultImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    resultImageViewCreateInfo.subresourceRange.levelCount = 1;
    resultImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    resultImageViewCreateInfo.subresourceRange.layerCount = 1;

    status = vkCreateImageView(this->device, &resultImageViewCreateInfo,
                               this->allocator, &this->sourceImageView);
    if (status != VK_SUCCESS) return status;

    resultImageViewCreateInfo.image = this->resultImage;
    status = vkCreateImageView(this->device, &resultImageViewCreateInfo,
                               this->allocator, &this->resultImageView);
    if (status != VK_SUCCESS) return status;

    VkSamplerCreateInfo sourceSamplerCreateInfo{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    sourceSamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    sourceSamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    sourceSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sourceSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sourceSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sourceSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sourceSamplerCreateInfo.mipLodBias = 0.0f;
    sourceSamplerCreateInfo.anisotropyEnable = VK_FALSE;
    sourceSamplerCreateInfo.maxAnisotropy = 0.0f;
    sourceSamplerCreateInfo.compareEnable = VK_FALSE;
    sourceSamplerCreateInfo.minLod = 0.0f;
    sourceSamplerCreateInfo.maxLod = 0.0f;
    sourceSamplerCreateInfo.borderColor = {};
    sourceSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    status = vkCreateSampler(this->device, &sourceSamplerCreateInfo,
                             this->allocator, &this->sourceImageSampler);
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
    loadReadBufferMemoryCreateInfo.memoryTypeIndex = this->findMemoryType(loadReadBufferMemReq,
                                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    status = vkAllocateMemory(this->device, &loadReadBufferMemoryCreateInfo,
                              this->allocator, &this->loadReadBufferMemory);
    if (status != VK_SUCCESS) return status;
    status = vkBindBufferMemory(this->device, this->loadReadBuffer, this->loadReadBufferMemory, 0);
    if (status != VK_SUCCESS) return status;
    this->setDebugName(this->loadReadBuffer, "loadReadBuffer");

    this->generateGrid(20, 20);//TODO: load resolution.
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
VkResult CMTVulkanBackend::createPSO(const Image &image) noexcept {
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2];
    shaderStageCreateInfo[0] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo[0].module = this->shaders.vertexShader;
    shaderStageCreateInfo[0].pName = "main";
    shaderStageCreateInfo[0].pSpecializationInfo = nullptr;

    shaderStageCreateInfo[1] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfo[1].module = this->shaders.fragmentShader;
    shaderStageCreateInfo[1].pName = "main";
    shaderStageCreateInfo[1].pSpecializationInfo = nullptr;

    VkVertexInputBindingDescription vertexInputBindings[] = {
            {0, sizeof(ShaderDataStructs::VertexData), VK_VERTEX_INPUT_RATE_VERTEX},
    };

    VkVertexInputAttributeDescription vertexAttributes[] = {
            {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ShaderDataStructs::VertexData, vPosition)},
    };

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = ARR_ELEM_COUNT(vertexInputBindings);
    vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindings;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = ARR_ELEM_COUNT(vertexAttributes);
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributes;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(image.width());
    viewport.height = static_cast<float>(image.height());
    viewport.minDepth = 1000.0f;


    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {static_cast<uint32_t>(image.width()),
                      static_cast<uint32_t>(image.height())};

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasClamp = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationStateCreateInfo.lineWidth = 1.0f;

    //----
    VkPipelineColorBlendStateCreateInfo cb{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    cb.flags = 0;
    cb.pNext = nullptr;

    VkPipelineColorBlendAttachmentState att_state[1];
    att_state[0].colorWriteMask = 0xf;
    att_state[0].blendEnable = VK_FALSE;
    att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
    att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
    att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    cb.attachmentCount = 1;
    cb.pAttachments = att_state;
    cb.logicOpEnable = VK_FALSE;
    cb.logicOp = VK_LOGIC_OP_NO_OP;
    cb.blendConstants[0] = 1.0f;
    cb.blendConstants[1] = 1.0f;
    cb.blendConstants[2] = 1.0f;
    cb.blendConstants[3] = 1.0f;


    VkPipelineDepthStencilStateCreateInfo ds{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable = VK_FALSE;
    ds.stencilTestEnable = VK_FALSE;
    ds.minDepthBounds = 0.0f;
    ds.maxDepthBounds = 1.0f;
    ds.stencilTestEnable = VK_FALSE;
    ds.front = {};
    ds.back = {};

    VkPipelineMultisampleStateCreateInfo ms{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    ms.pSampleMask = nullptr;
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ms.sampleShadingEnable = VK_FALSE;
    ms.alphaToCoverageEnable = VK_FALSE;
    ms.alphaToOneEnable = VK_FALSE;
    ms.minSampleShading = 0.0;

    VkDynamicState dynamicStateEnables[2];// Viewport + Scissor
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    memset(dynamicStateEnables, 0, sizeof(dynamicStateEnables));
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 0;

    //----

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipelineCreateInfo.stageCount = ARR_ELEM_COUNT(shaderStageCreateInfo);
    pipelineCreateInfo.pStages = shaderStageCreateInfo;
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &ms;
    pipelineCreateInfo.pDepthStencilState = &ds;
    pipelineCreateInfo.pColorBlendState = &cb;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = this->pipelineLayout;
    pipelineCreateInfo.renderPass = this->renderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;

    return vkCreateGraphicsPipelines(this->device,
                                     VK_NULL_HANDLE,//TODO: load cache
                                     1,
                                     &pipelineCreateInfo,
                                     this->allocator,
                                     &this->pipeline);
}

VkResult CMTVulkanBackend::uploadResources(const Image &image) noexcept {
    VkResult status;
    void *mappedData = nullptr;
    status = vkMapMemory(this->device, this->loadReadBufferMemory, 0, VK_WHOLE_SIZE, 0, &mappedData);
    if (status != VK_SUCCESS) return status;
    const unsigned char *imageData = image.constBits();
    memcpy(mappedData, imageData, image.sizeInBytes());//todo: careful

    VkMappedMemoryRange flushRange{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
    flushRange.memory = this->loadReadBufferMemory;
    flushRange.offset = 0;
    flushRange.size = VK_WHOLE_SIZE;
    status = vkFlushMappedMemoryRanges(this->device, 1, &flushRange);
    if (status != VK_SUCCESS) return status;
    vkUnmapMemory(this->device, this->loadReadBufferMemory);

    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = this->queueIndex;
    barrier.dstQueueFamilyIndex = this->queueIndex;
    barrier.image = this->sourceImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;


    vkCmdPipelineBarrier(this->commandBuffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
            static_cast<std::uint32_t>(image.width()),
            static_cast<std::uint32_t>(image.height()),
            1};

    vkCmdCopyBufferToImage(this->commandBuffer, this->loadReadBuffer, this->sourceImage,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    vkCmdPipelineBarrier(this->commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);
    return VK_SUCCESS;
}

void CMTVulkanBackend::bindResources() noexcept {
    VkDescriptorBufferInfo fromBufferInfo = {this->fromStrokeBuffer, 0, VK_WHOLE_SIZE};
    VkWriteDescriptorSet fromStrokeWrite{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    fromStrokeWrite.dstSet = this->descriptorSets[0];
    fromStrokeWrite.dstBinding = 0;
    fromStrokeWrite.dstArrayElement = 0;
    fromStrokeWrite.descriptorCount = 1;
    fromStrokeWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    fromStrokeWrite.pBufferInfo = &fromBufferInfo;

    VkDescriptorBufferInfo toBufferInfo = {this->toStrokeBuffer, 0, VK_WHOLE_SIZE};
    VkWriteDescriptorSet toStrokeWrite{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    toStrokeWrite.dstSet = this->descriptorSets[0];
    toStrokeWrite.dstBinding = 0;
    toStrokeWrite.dstArrayElement = 0;
    toStrokeWrite.descriptorCount = 1;
    toStrokeWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    toStrokeWrite.pBufferInfo = &toBufferInfo;

    VkDescriptorImageInfo textureInfos[] = {
            {this->sourceImageSampler, this->sourceImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
    };

    VkWriteDescriptorSet texturesWrite{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    texturesWrite.dstSet = this->descriptorSets[0];
    texturesWrite.dstBinding = 2;
    texturesWrite.dstArrayElement = 0;
    texturesWrite.descriptorCount = ARR_ELEM_COUNT(textureInfos);
    texturesWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    texturesWrite.pImageInfo = textureInfos;

    VkWriteDescriptorSet descriptorWrites[] = {fromStrokeWrite, toStrokeWrite, texturesWrite};

    vkUpdateDescriptorSets(this->device,
                           ARR_ELEM_COUNT(descriptorWrites), descriptorWrites,
                           0, nullptr);
}
VkResult CMTVulkanBackend::readbackPrepareResources(const Image &image) noexcept {
    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = this->queueIndex;
    barrier.dstQueueFamilyIndex = this->queueIndex;
    barrier.image = this->resultImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;


    vkCmdPipelineBarrier(this->commandBuffer,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);


    VkBufferImageCopy bufferImageCopy{};
    bufferImageCopy.bufferOffset = 0;
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.bufferImageHeight = 0;
    bufferImageCopy.imageOffset = {0, 0, 0};
    bufferImageCopy.imageExtent = {static_cast<std::uint32_t>(image.width()),
                                   static_cast<std::uint32_t>(image.height()),
                                   1};
    bufferImageCopy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};

    vkCmdCopyImageToBuffer(this->commandBuffer, this->resultImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           this->loadReadBuffer, 1, &bufferImageCopy);

    VkBufferMemoryBarrier bufferMemoryBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
    bufferMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    bufferMemoryBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
    bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.buffer = this->loadReadBuffer;
    bufferMemoryBarrier.offset = 0;
    bufferMemoryBarrier.size = VK_WHOLE_SIZE;

    vkCmdPipelineBarrier(this->commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT,
                         0,
                         0, nullptr,
                         1, &bufferMemoryBarrier,
                         0, nullptr);
    return VK_SUCCESS;
}

VkResult CMTVulkanBackend::generateGrid(std::size_t dimensionX, std::size_t dimensionY) noexcept {
    std::vector<ShaderDataStructs::VertexData> points{};
    //    points.reserve(dimensionX * dimensionY);
    //    for (std::size_t x = 0; x < dimensionX; ++x) {
    //        for (std::size_t y = 0; y < dimensionY; ++y) {
    //
    //        }
    //    }
    points.push_back({{0.0f, 0.0f}});
    points.push_back({{0.0f, 1.0f}});
    points.push_back({{1.0f, 0.0f}});

    points.push_back({{1.0f, 0.0f}});
    points.push_back({{0.0f, 1.0f}});
    points.push_back({{1.0f, 1.0f}});

    VkResult status;
    VkBufferCreateInfo vertexBufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    vertexBufferCreateInfo.size = points.size() * sizeof(ShaderDataStructs::VertexData);
    vertexBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vertexBufferCreateInfo.queueFamilyIndexCount = 0;
    vertexBufferCreateInfo.pQueueFamilyIndices = nullptr;
    status = vkCreateBuffer(this->device, &vertexBufferCreateInfo,
                            this->allocator, &this->vertexBuffer);
    if (status != VK_SUCCESS) return status;
    VkMemoryRequirements vertexBufferMemReq{};
    vkGetBufferMemoryRequirements(this->device, this->vertexBuffer, &vertexBufferMemReq);


    VkMemoryAllocateInfo vertexBufferMemoryCreateInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    vertexBufferMemoryCreateInfo.allocationSize = vertexBufferMemReq.size;
    vertexBufferMemoryCreateInfo.memoryTypeIndex = this->findMemoryType(vertexBufferMemReq,
                                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    status = vkAllocateMemory(this->device, &vertexBufferMemoryCreateInfo,
                              this->allocator, &this->vertexBufferMemory);
    vkBindBufferMemory(this->device, this->vertexBuffer, this->vertexBufferMemory, 0);
    this->setDebugName(this->vertexBuffer, "vertexBuffer");

    void *mappedData = nullptr;
    status = vkMapMemory(this->device, this->vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, &mappedData);
    if (status != VK_SUCCESS) return status;
    memcpy(mappedData, points.data(), points.size() * sizeof(ShaderDataStructs::VertexData));
    vkUnmapMemory(this->device, this->vertexBufferMemory);

    return status;
}
VkResult CMTVulkanBackend::readbackResources(Image &image) noexcept {
    VkResult status;
    void *mappedData = nullptr;
    status = vkMapMemory(this->device, this->loadReadBufferMemory, 0, VK_WHOLE_SIZE, 0, &mappedData);
    if (status != VK_SUCCESS) return status;
    VkMappedMemoryRange invalidateRange{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
    invalidateRange.memory = this->loadReadBufferMemory;
    invalidateRange.offset = 0;
    invalidateRange.size = VK_WHOLE_SIZE;
    status = vkInvalidateMappedMemoryRanges(this->device, 1, &invalidateRange);
    if (status != VK_SUCCESS) return status;
    auto imageData = static_cast<const uint32_t *>(mappedData);

    for (size_t x = 0; x < image.width(); ++x) {
        for (size_t y = 0; y < image.height(); ++y) {
            uint32_t packed = imageData[y * image.width() + x];
            unsigned char r = packed >> 0;
            unsigned char g = packed >> 8;
            unsigned char b = packed >> 16;
            unsigned char a = packed >> 24;
            if (packed) {
                printf("AAAAA");
            }
            image.setPixelColor(QPoint(x, y), QColor{r, g, b});
        }
    }


    vkUnmapMemory(this->device, this->loadReadBufferMemory);

    return VK_SUCCESS;
}

void CMTVulkanBackend::setDebugName(VkBuffer buffer, const char *name) const noexcept {
#ifdef IMT_DEBUG
    auto vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(this->instance, "vkSetDebugUtilsObjectNameEXT"));
    VkDebugUtilsObjectNameInfoEXT info{VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
    info.objectType = VK_OBJECT_TYPE_BUFFER;
    info.objectHandle = reinterpret_cast<uint64_t>(buffer);
    info.pObjectName = name;
    vkSetDebugUtilsObjectNameEXT(this->device, &info);
#endif//IMT_DEBUG
}

void CMTVulkanBackend::setDebugName(VkImage image, const char *name) const noexcept {
#ifdef IMT_DEBUG
    auto vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(this->instance, "vkSetDebugUtilsObjectNameEXT"));
    VkDebugUtilsObjectNameInfoEXT info{VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
    info.objectType = VK_OBJECT_TYPE_IMAGE;
    info.objectHandle = reinterpret_cast<uint64_t>(image);
    info.pObjectName = name;
    vkSetDebugUtilsObjectNameEXT(this->device, &info);
#endif//IMT_DEBUG
}
