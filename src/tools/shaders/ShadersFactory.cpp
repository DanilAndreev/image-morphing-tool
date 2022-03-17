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

#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/Logger.h"
#include "SPIRV/SpvTools.h"
#include "StandAlone/DirStackFileIncluder.h"
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

#include <glslang/Include/ShHandle.h>

#include <glslang/Include/ResourceLimits.h>
#include <glslang/MachineIndependent/Versions.h>
#include <iostream>

const TBuiltInResource DefaultTBuiltInResource = {
        /* .MaxLights = */ 32,
        /* .MaxClipPlanes = */ 6,
        /* .MaxTextureUnits = */ 32,
        /* .MaxTextureCoords = */ 32,
        /* .MaxVertexAttribs = */ 64,
        /* .MaxVertexUniformComponents = */ 4096,
        /* .MaxVaryingFloats = */ 64,
        /* .MaxVertexTextureImageUnits = */ 32,
        /* .MaxCombinedTextureImageUnits = */ 80,
        /* .MaxTextureImageUnits = */ 32,
        /* .MaxFragmentUniformComponents = */ 4096,
        /* .MaxDrawBuffers = */ 32,
        /* .MaxVertexUniformVectors = */ 128,
        /* .MaxVaryingVectors = */ 8,
        /* .MaxFragmentUniformVectors = */ 16,
        /* .MaxVertexOutputVectors = */ 16,
        /* .MaxFragmentInputVectors = */ 15,
        /* .MinProgramTexelOffset = */ -8,
        /* .MaxProgramTexelOffset = */ 7,
        /* .MaxClipDistances = */ 8,
        /* .MaxComputeWorkGroupCountX = */ 65535,
        /* .MaxComputeWorkGroupCountY = */ 65535,
        /* .MaxComputeWorkGroupCountZ = */ 65535,
        /* .MaxComputeWorkGroupSizeX = */ 1024,
        /* .MaxComputeWorkGroupSizeY = */ 1024,
        /* .MaxComputeWorkGroupSizeZ = */ 64,
        /* .MaxComputeUniformComponents = */ 1024,
        /* .MaxComputeTextureImageUnits = */ 16,
        /* .MaxComputeImageUniforms = */ 8,
        /* .MaxComputeAtomicCounters = */ 8,
        /* .MaxComputeAtomicCounterBuffers = */ 1,
        /* .MaxVaryingComponents = */ 60,
        /* .MaxVertexOutputComponents = */ 64,
        /* .MaxGeometryInputComponents = */ 64,
        /* .MaxGeometryOutputComponents = */ 128,
        /* .MaxFragmentInputComponents = */ 128,
        /* .MaxImageUnits = */ 8,
        /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
        /* .MaxCombinedShaderOutputResources = */ 8,
        /* .MaxImageSamples = */ 0,
        /* .MaxVertexImageUniforms = */ 0,
        /* .MaxTessControlImageUniforms = */ 0,
        /* .MaxTessEvaluationImageUniforms = */ 0,
        /* .MaxGeometryImageUniforms = */ 0,
        /* .MaxFragmentImageUniforms = */ 8,
        /* .MaxCombinedImageUniforms = */ 8,
        /* .MaxGeometryTextureImageUnits = */ 16,
        /* .MaxGeometryOutputVertices = */ 256,
        /* .MaxGeometryTotalOutputComponents = */ 1024,
        /* .MaxGeometryUniformComponents = */ 1024,
        /* .MaxGeometryVaryingComponents = */ 64,
        /* .MaxTessControlInputComponents = */ 128,
        /* .MaxTessControlOutputComponents = */ 128,
        /* .MaxTessControlTextureImageUnits = */ 16,
        /* .MaxTessControlUniformComponents = */ 1024,
        /* .MaxTessControlTotalOutputComponents = */ 4096,
        /* .MaxTessEvaluationInputComponents = */ 128,
        /* .MaxTessEvaluationOutputComponents = */ 128,
        /* .MaxTessEvaluationTextureImageUnits = */ 16,
        /* .MaxTessEvaluationUniformComponents = */ 1024,
        /* .MaxTessPatchComponents = */ 120,
        /* .MaxPatchVertices = */ 32,
        /* .MaxTessGenLevel = */ 64,
        /* .MaxViewports = */ 16,
        /* .MaxVertexAtomicCounters = */ 0,
        /* .MaxTessControlAtomicCounters = */ 0,
        /* .MaxTessEvaluationAtomicCounters = */ 0,
        /* .MaxGeometryAtomicCounters = */ 0,
        /* .MaxFragmentAtomicCounters = */ 8,
        /* .MaxCombinedAtomicCounters = */ 8,
        /* .MaxAtomicCounterBindings = */ 1,
        /* .MaxVertexAtomicCounterBuffers = */ 0,
        /* .MaxTessControlAtomicCounterBuffers = */ 0,
        /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
        /* .MaxGeometryAtomicCounterBuffers = */ 0,
        /* .MaxFragmentAtomicCounterBuffers = */ 1,
        /* .MaxCombinedAtomicCounterBuffers = */ 1,
        /* .MaxAtomicCounterBufferSize = */ 16384,
        /* .MaxTransformFeedbackBuffers = */ 4,
        /* .MaxTransformFeedbackInterleavedComponents = */ 64,
        /* .MaxCullDistances = */ 8,
        /* .MaxCombinedClipAndCullDistances = */ 8,
        /* .MaxSamples = */ 4,
        /* .maxMeshOutputVerticesNV = */ 256,
        /* .maxMeshOutputPrimitivesNV = */ 512,
        /* .maxMeshWorkGroupSizeX_NV = */ 32,
        /* .maxMeshWorkGroupSizeY_NV = */ 1,
        /* .maxMeshWorkGroupSizeZ_NV = */ 1,
        /* .maxTaskWorkGroupSizeX_NV = */ 32,
        /* .maxTaskWorkGroupSizeY_NV = */ 1,
        /* .maxTaskWorkGroupSizeZ_NV = */ 1,
        /* .maxMeshViewCountNV = */ 4,
        /* .maxDualSourceDrawBuffersEXT = */ 1,

        /* .limits = */ {
                /* .nonInductiveForLoops = */ 1,
                /* .whileLoops = */ 1,
                /* .doWhileLoops = */ 1,
                /* .generalUniformIndexing = */ 1,
                /* .generalAttributeMatrixVectorIndexing = */ 1,
                /* .generalVaryingIndexing = */ 1,
                /* .generalSamplerIndexing = */ 1,
                /* .generalVariableIndexing = */ 1,
                /* .generalConstantMatrixVectorIndexing = */ 1,
        }};


std::vector<std::uint32_t> ShadersFactory::getSPIRVfromGLSL(EShLanguage stage) {
    bool result = glslang::InitializeProcess();
    if (!result) {
        throw Exceptions::ShaderGlslangInitializationError("Failed to initialize glslang.");
    }
    glslang::TShader shader{stage};
    const char *code = this->shaderCode.c_str();
    shader.setStrings(&code, 1);
    shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 450);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);

    DirStackFileIncluder Includer{};
    TBuiltInResource resources = DefaultTBuiltInResource;
    std::string preprocessedGLSL{};
    result = shader.preprocess(
            &resources,
            450,
            ECoreProfile,
            false,
            false,
            EShMsgDefault,
            &preprocessedGLSL,
            Includer);
    if (!result) {
        throw Exceptions::ShaderGlslangInitializationError(QByteArray(shader.getInfoLog()) + "\n" + shader.getInfoDebugLog());
    }
    const char *preprocessedCStr = preprocessedGLSL.c_str();
    shader.setStrings(&preprocessedCStr, 1);

    result = shader.parse(
            &resources,
            450,
            false,
            EShMsgDefault);
    if (!result) {
        throw Exceptions::ShaderGlslangInitializationError(QByteArray(shader.getInfoLog()) + "\n" + shader.getInfoDebugLog());
    }

    glslang::TProgram program{};
    program.addShader(&shader);
    result = program.link(EShMsgDefault);
    if (!result) {
        throw Exceptions::ShaderGlslangInitializationError(QByteArray(program.getInfoLog()) + "\n" + program.getInfoDebugLog());
    }

    spv::SpvBuildLogger logger{};
    glslang::SpvOptions spvOptions;
    spvOptions.validate = true;
    std::vector<uint32_t> spirv{};
    const glslang::TIntermediate *intermediate = program.getIntermediate(stage);
    glslang::GlslangToSpv(*intermediate, spirv, &logger, &spvOptions);
    return spirv;
}
