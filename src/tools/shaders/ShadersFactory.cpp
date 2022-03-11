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

#include "tools/shaders/ShadersFactory.h"
#include <fstream>

#include "glslang_c_interface.h"

void ShadersFactory::loadFileOnPath(const std::string &filename) {
    std::ifstream stream{filename};
    this->loadFile(stream);
    stream.close();
}

void ShadersFactory::loadFile(std::istream &stream) {
    std::string text((std::istreambuf_iterator<char>(stream)),
                     std::istreambuf_iterator<char>());
    this->loadFile(text);
}

void ShadersFactory::loadFile(const std::string &shaderText) {
    this->shaderCode = shaderText;
}

std::vector<std::uint32_t> ShadersFactory::getSPIRVfromGLSL(glslang_stage_t stage) {
    glslang_resource_s resource{};
    const glslang_input_t input = {
            .language = GLSLANG_SOURCE_GLSL,
            .stage = stage,
            .client = GLSLANG_CLIENT_VULKAN,
            .client_version = GLSLANG_TARGET_VULKAN_1_0,
            .target_language = GLSLANG_TARGET_SPV,
            .target_language_version = GLSLANG_TARGET_SPV_1_3,
            .code = this->shaderCode.c_str(),
            .default_version = 450,
            .default_profile = GLSLANG_CORE_PROFILE,
            .force_default_version_and_profile = false,
            .forward_compatible = true,
            .messages = GLSLANG_MSG_ENHANCED,
            .resource = &resource,
    };

    glslang_initialize_process();

    glslang_shader_t *shader = glslang_shader_create(&input);

    if (!glslang_shader_preprocess(shader, &input)) {
        // use glslang_shader_get_info_log() and glslang_shader_get_info_debug_log()
    }

    if (!glslang_shader_parse(shader, &input)) {
        // use glslang_shader_get_info_log() and glslang_shader_get_info_debug_log()
    }

    glslang_program_t *program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
        // use glslang_program_get_info_log() and glslang_program_get_info_debug_log();
    }

    glslang_program_SPIRV_generate(program, input.stage);

    if (glslang_program_SPIRV_get_messages(program)) {
        printf("%s", glslang_program_SPIRV_get_messages(program));
    }

    std::vector<std::uint32_t> spirv{};
    spirv.resize(glslang_program_SPIRV_get_size(program));
    memcpy(spirv.data(), glslang_program_SPIRV_get_ptr(program), spirv.size() * sizeof(spirv[0]));

    glslang_program_delete(program);
    glslang_shader_delete(shader);
    return spirv;
}
