//
// Created by andyroiiid on 12/20/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>

class ShaderCompiler {
public:
    ShaderCompiler();

    ~ShaderCompiler();

    ShaderCompiler(const ShaderCompiler &) = delete;

    ShaderCompiler &operator=(const ShaderCompiler &) = delete;

    ShaderCompiler(ShaderCompiler &&) = delete;

    ShaderCompiler &operator=(ShaderCompiler &&) = delete;

    void SetPreamble(std::string preamble);

    bool Compile(vk::ShaderStageFlagBits stage, const char *source, std::vector<uint32_t> &spirv);

private:
    std::string m_preamble;
};
