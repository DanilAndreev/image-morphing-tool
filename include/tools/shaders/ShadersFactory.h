// Copyright (c) 2022-2022.
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

#ifndef IMAGE_MORPHING_TOOL_SHADERSFACTORY_H
#define IMAGE_MORPHING_TOOL_SHADERSFACTORY_H

#include <string>
#include <vector>
#include <istream>
#include <glslang_c_shader_types.h>

class ShadersFactory {
protected:
    std::string shaderCode{};
public:
    void loadFileOnPath(const std::string& filename);
    void loadFile(std::istream& stream);
    void loadFile(const std::string& shaderText);

public:
    std::vector<std::uint32_t> getSPIRVfromGLSL(glslang_stage_t stage);
};

#endif //IMAGE_MORPHING_TOOL_SHADERSFACTORY_H
