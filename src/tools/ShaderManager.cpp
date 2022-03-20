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

#include "tools/ShaderManager.h"
#include "tools/shaders/ShadersFactory.h"

bool ShaderManager::validateAndApply(const std::string& inGlsl) noexcept {
    try {
        this->validateAndApplyInternal(inGlsl);
    } catch (std::exception& err) {
        this->valid = false;
        if (this->shaderModule)
            vkDestroyShaderModule(this->device, this->shaderModule, this->allocator);
        this->shaderModule = VK_NULL_HANDLE;
        this->spirv.clear();
        this->error = err.what();
    }
    return this->valid;
}

const std::string&ShaderManager::getError() const noexcept {
    return this->error;
}

bool ShaderManager::isValid() const noexcept {
    return this->valid;
}

void ShaderManager::validateAndApplyInternal(const std::string &inGlsl) {
    this->glsl = inGlsl;
    ShadersFactory factory{};
    factory.loadFile(this->glsl);

    EShLanguage glslangStage;
    switch (this->stage) {
        case ShaderStage::VertexShader:
            glslangStage = EShLangVertex;
            break;
        case ShaderStage::FragmentShader:
            glslangStage = EShLangFragment;
            break;
    }

    this->spirv = factory.getSPIRVfromGLSL(glslangStage);

    VkShaderModuleCreateInfo createInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = this->spirv.size() * sizeof(std::uint32_t);
    createInfo.pCode = this->spirv.data();
    VkResult status = vkCreateShaderModule(device, &createInfo, allocator, &this->shaderModule);
    if (status != VK_SUCCESS) throw Exceptions::Exception("Failed to create vulkan shader module.");
    this->valid = true;
}
bool ShaderManager::validateAndApply(const std::vector<std::uint8_t> &inGlsl) noexcept {
    std::string code{inGlsl.begin(), inGlsl.end()};
    code.push_back('\0');
    return this->validateAndApply(code);
}
