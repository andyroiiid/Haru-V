//
// Created by andyroiiid on 12/20/2022.
//

#include "vulkan/ShaderCompiler.h"

#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include "core/Debug.h"
#include "file/FileSystem.h"

struct ShaderIncluder : glslang::TShader::Includer {
    explicit ShaderIncluder(std::string includesDir)
        : m_includesDir(std::move(includesDir)) {}

    IncludeResult *includeLocal(const char *, const char *, size_t) override {
        // relative paths are not supported yet
        return nullptr;
    }

    IncludeResult *includeSystem(const char *headerName, const char *, size_t) override { return include(headerName); }

    void releaseInclude(IncludeResult *result) override { delete result; }

private:
    IncludeResult *include(const std::string &headerName) {
        auto pair = m_headerFiles.find(headerName);
        if (pair == m_headerFiles.end()) {
            DebugInfo("Loading shader header file <{}> into cache.", headerName);
            pair = m_headerFiles.emplace(headerName, FileSystem::Read(m_includesDir + headerName)).first;
        }
        auto &content = pair->second;
        return new IncludeResult(headerName, content.c_str(), content.length(), nullptr);
    }

    std::string m_includesDir;

    std::map<std::string, std::string> m_headerFiles;
};

ShaderCompiler::ShaderCompiler(const std::string &includesDir) {
    DebugInfo("glslang version: {}", glslang::GetGlslVersionString());
    DebugCheckCritical(glslang::InitializeProcess(), "Failed to initialize glslang.");
    m_includer = std::make_unique<ShaderIncluder>(includesDir);
}

ShaderCompiler::~ShaderCompiler() {
    m_includer.reset();
    glslang::FinalizeProcess();
}

static inline EShLanguage GetShaderStageLanguage(vk::ShaderStageFlagBits stage) {
    switch (stage) {
    case vk::ShaderStageFlagBits::eVertex:
        return EShLangVertex;
    case vk::ShaderStageFlagBits::eGeometry:
        return EShLangGeometry;
    case vk::ShaderStageFlagBits::eFragment:
        return EShLangFragment;
    case vk::ShaderStageFlagBits::eCompute:
        return EShLangCompute;
    default:
        return EShLangCount;
    }
}

static inline const char *GetShaderStageName(vk::ShaderStageFlagBits stage) {
    switch (stage) {
    case vk::ShaderStageFlagBits::eVertex:
        return "vertex";
    case vk::ShaderStageFlagBits::eGeometry:
        return "geometry";
    case vk::ShaderStageFlagBits::eFragment:
        return "fragment";
    case vk::ShaderStageFlagBits::eCompute:
        return "compute";
    default:
        return "unsupported stage";
    }
}

std::vector<uint32_t> ShaderCompiler::Compile(vk::ShaderStageFlagBits stage, const std::string &source) {
    const EShLanguage glslStage = GetShaderStageLanguage(stage);
    const char       *stageName = GetShaderStageName(stage);

    glslang::TShader shader(glslStage);
    const char      *sourceCStr = source.c_str();
    shader.setStrings(&sourceCStr, 1);
    shader.setPreamble("#extension GL_GOOGLE_include_directive : require\n");
    shader.setEnvInput(glslang::EShSourceGlsl, glslStage, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
    shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_0);

    if (!shader.parse(GetDefaultResources(), 100, false, EShMsgDefault, *m_includer)) {
        DebugError("Failed to parse {} shader: {}", stageName, shader.getInfoLog());
        return {};
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(EShMsgDefault)) {
        DebugError("Failed to link {} shader program: {}", stageName, program.getInfoLog());
        return {};
    }

    std::vector<uint32_t> spirv;
    glslang::GlslangToSpv(*program.getIntermediate(glslStage), spirv);
    return spirv;
}

std::vector<uint32_t> ShaderCompiler::CompileFromFile(vk::ShaderStageFlagBits stage, const std::string &filename) {
    const char *stageName = GetShaderStageName(stage);
    DebugInfo("Compiling {} shader: {}", stageName, filename);
    return Compile(stage, FileSystem::Read(filename));
}
