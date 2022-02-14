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

#ifndef IMAGE_MORPHING_TOOL_CMT_VULKANBACKEND_H
#define IMAGE_MORPHING_TOOL_CMT_VULKANBACKEND_H

#include <vector>
#include <vulkan/vulkan.h>

class CMTVulkanBackend {
protected:
    struct ShaderModules {
        VkShaderModule vertexShader = VK_NULL_HANDLE;
        VkShaderModule fragmentShader = VK_NULL_HANDLE;
    };

protected:
    VkAllocationCallbacks* allocator = nullptr;

    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue deviceQueue = VK_NULL_HANDLE;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    ShaderModules shaders{};

    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets{VK_NULL_HANDLE};

public:
    CMTVulkanBackend() = default;
    CMTVulkanBackend(const CMTVulkanBackend&) = delete;
    ~CMTVulkanBackend() = default;

public:
    VkResult initialize() noexcept;
    void release() noexcept;

    VkResult initializeRenderer() noexcept;
    void releaseRenderer() noexcept;
private:
    VkResult selectPhysicalDevice(uint32_t* outQueueFamilyIndex) noexcept;
    VkResult initializeShaderModules() noexcept;
    void releaseShaderModules() noexcept;
    VkResult createPipelineLayout(VkPipelineLayout* outPipelineLayout, std::vector<VkDescriptorSetLayout>* outSetLayouts) noexcept;
};

#endif//IMAGE_MORPHING_TOOL_CMT_VULKANBACKEND_H
