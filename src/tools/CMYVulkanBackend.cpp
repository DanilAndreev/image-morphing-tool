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

    for (const VkDescriptorSetLayout &setLayout : setLayouts) {
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
