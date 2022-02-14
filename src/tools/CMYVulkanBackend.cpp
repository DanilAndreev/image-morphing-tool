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
#include <vector>

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
                                   VkShaderModule* outShaderModule) noexcept {
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
