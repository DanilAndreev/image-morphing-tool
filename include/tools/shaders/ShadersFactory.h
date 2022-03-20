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

#include "core/exceptions/Exception.h"
#include "glslang/Public/ShaderLang.h"
#include <istream>
#include <string>
#include <vector>

namespace Exceptions {
    class ShaderTranslationError : public Exception {
    public:
        explicit ShaderTranslationError(const QByteArray &message = "") noexcept : Exception(message) {}
    };
    class ShaderLinkingError : public ShaderTranslationError {
    public:
        explicit ShaderLinkingError(const QByteArray &message = "") noexcept : ShaderTranslationError(message) {}
    };
    class ShaderGlslangInitializationError : public ShaderTranslationError {
    public:
        explicit ShaderGlslangInitializationError(const QByteArray &message = "") noexcept : ShaderTranslationError(message) {}
    };
    class ShaderParsingError : public ShaderTranslationError {
    public:
        explicit ShaderParsingError(const QByteArray &message = "") noexcept : ShaderTranslationError(message) {}
    };
}// namespace Exceptions

class ShadersFactory {
protected:
    std::string shaderCode{};

public:
    void loadFileOnPath(const std::string &filename);
    void loadFile(std::istream &stream);
    void loadFile(const std::string &shaderText);
public:
    std::vector<std::uint32_t> getSPIRVfromGLSL(EShLanguage stage);
};

#endif//IMAGE_MORPHING_TOOL_SHADERSFACTORY_H
