//
// Created by andyroiiid on 12/20/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>

struct ShaderIncluder;

class ShaderCompiler {
public:
    ShaderCompiler();

    ~ShaderCompiler();

    ShaderCompiler(const ShaderCompiler &) = delete;

    ShaderCompiler &operator=(const ShaderCompiler &) = delete;

    ShaderCompiler(ShaderCompiler &&) = delete;

    ShaderCompiler &operator=(ShaderCompiler &&) = delete;

    std::vector<uint32_t> Compile(vk::ShaderStageFlagBits stage, const std::string &source);

    std::vector<uint32_t> CompileFromFile(vk::ShaderStageFlagBits stage, const std::string &filename);

private:
    std::unique_ptr<ShaderIncluder> m_includer;
};
