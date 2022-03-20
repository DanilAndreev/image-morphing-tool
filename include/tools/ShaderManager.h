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

#ifndef IMAGE_MORPHING_TOOL_SHADERSMANAGER_H
#define IMAGE_MORPHING_TOOL_SHADERSMANAGER_H

#include <event_emitter.h>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class ShaderManager : public events::event_emitter {
public:
    class ShaderUpdateEvent : public events::event_base {
    public:
        const std::string& glsl;
        const std::string& error;
        bool valid;
    public:
        ShaderUpdateEvent(const ShaderManager& manager)
            : events::event_base(), glsl(manager.glsl), valid(manager.valid), error(manager.error) {}
    };
public:
    static constexpr const char* SHADER_UPDATE_EVENT = "shader-update";
public:
    enum class ShaderStage {
        VertexShader,
        FragmentShader,
    };

protected:
    bool valid = false;
    std::string error{};

public:
    std::string glsl{};
    std::vector<uint32_t> spirv{};
    VkShaderModule shaderModule = VK_NULL_HANDLE;
    ShaderStage stage;

protected:
    VkDevice device = VK_NULL_HANDLE;
    VkAllocationCallbacks *allocator = nullptr;

public:
    explicit ShaderManager(ShaderStage stage, VkDevice device, VkAllocationCallbacks *allocator)
        : stage(stage), device(device), allocator(allocator) {}

public:
    bool validateAndApply(const std::string &inGlsl) noexcept;
    bool validateAndApply(const std::vector<std::uint8_t> &inGlsl) noexcept;
    [[nodiscard]] const std::string &getError() const noexcept;
    [[nodiscard]] bool isValid() const noexcept;

private:
    void validateAndApplyInternal(const std::string &inGlsl);
};

#endif//IMAGE_MORPHING_TOOL_SHADERSMANAGER_H
