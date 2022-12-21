//
// Created by andyroiiid on 12/20/2022.
//

#include "vulkan/ShaderCompiler.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include "core/Debug.h"

ShaderCompiler::ShaderCompiler() {
    DebugInfo("glslang version: {}", glslang::GetGlslVersionString());
    DebugCheckCritical(glslang::InitializeProcess(), "Failed to initialize glslang.");
}

ShaderCompiler::~ShaderCompiler() {
    glslang::FinalizeProcess();
}

void ShaderCompiler::SetPreamble(std::string preamble) {
    m_preamble = std::move(preamble);
}

static inline std::tuple<EShLanguage, const char *> GetShaderStageLanguageAndName(vk::ShaderStageFlagBits stage) {
    switch (stage) {
        case vk::ShaderStageFlagBits::eVertex:
            return {EShLangVertex, "vertex"};
        case vk::ShaderStageFlagBits::eGeometry:
            return {EShLangGeometry, "geometry"};
        case vk::ShaderStageFlagBits::eFragment:
            return {EShLangFragment, "fragment"};
        case vk::ShaderStageFlagBits::eCompute:
            return {EShLangCompute, "compute"};
        default:
            return {{}, "unsupported stage"};
    }
}

bool ShaderCompiler::Compile(vk::ShaderStageFlagBits stage, const char *source, std::vector<uint32_t> &spirv) {
    auto [glslStage, stageName] = GetShaderStageLanguageAndName(stage);

    glslang::TShader shader(glslStage);
    shader.setStrings(&source, 1);
    shader.setPreamble(m_preamble.c_str());
    shader.setEnvInput(glslang::EShSourceGlsl, glslStage, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
    shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_0);

    if (!shader.parse(GetDefaultResources(), 100, false, EShMsgDefault)) {
        DebugError("Failed to parse {} shader: {}", stageName, shader.getInfoLog());
        return false;
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(EShMsgDefault)) {
        DebugError("Failed to link {} shader program: {}", stageName, program.getInfoLog());
        return false;
    }

    glslang::GlslangToSpv(*program.getIntermediate(glslStage), spirv);
    return true;
}
