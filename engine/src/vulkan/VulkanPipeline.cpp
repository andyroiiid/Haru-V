//
// Created by andyroiiid on 12/22/2022.
//

#include "vulkan/VulkanPipeline.h"

#include "vulkan/ShaderCompiler.h"
#include "vulkan/VulkanDevice.h"
#include "vulkan/VulkanPipelineConfig.h"

VulkanPipeline::VulkanPipeline(
    VulkanDevice                                                       &device,
    ShaderCompiler                                                     &compiler,
    const std::initializer_list<vk::DescriptorSetLayout>               &descriptorSetLayouts,
    const std::initializer_list<vk::PushConstantRange>                 &pushConstantRanges,
    const vk::PipelineVertexInputStateCreateInfo                       *vertexInput,
    const std::string                                                  &pipelineConfigFile,
    const std::initializer_list<vk::PipelineColorBlendAttachmentState> &attachmentColorBlends,
    vk::RenderPass                                                      renderPass,
    uint32_t                                                            subpass
)
    : m_device(&device) {
    m_pipelineLayout = m_device->CreatePipelineLayout(descriptorSetLayouts, pushConstantRanges);

    const VulkanPipelineConfig pipelineConfig(pipelineConfigFile);

    const std::vector<uint32_t> vertexSpirv = compiler.CompileFromFile(vk::ShaderStageFlagBits::eVertex, pipelineConfig.VertexShader);
    m_vertexShaderModule                    = m_device->CreateShaderModule(vertexSpirv);
    if (!pipelineConfig.GeometryShader.empty()) {
        const std::vector<uint32_t> geometrySpirv = compiler.CompileFromFile(vk::ShaderStageFlagBits::eGeometry, pipelineConfig.GeometryShader);
        m_geometryShaderModule                    = m_device->CreateShaderModule(geometrySpirv);
    }
    const std::vector<uint32_t> fragmentSpirv = compiler.CompileFromFile(vk::ShaderStageFlagBits::eFragment, pipelineConfig.FragmentShader);
    m_fragmentShaderModule                    = m_device->CreateShaderModule(fragmentSpirv);

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

    shaderStages.emplace_back(vk::PipelineShaderStageCreateFlags{}, vk::ShaderStageFlagBits::eVertex, m_vertexShaderModule, "main");
    if (!pipelineConfig.GeometryShader.empty()) {
        shaderStages.emplace_back(vk::PipelineShaderStageCreateFlags{}, vk::ShaderStageFlagBits::eGeometry, m_geometryShaderModule, "main");
    }
    shaderStages.emplace_back(vk::PipelineShaderStageCreateFlags{}, vk::ShaderStageFlagBits::eFragment, m_fragmentShaderModule, "main");

    m_pipeline =
        m_device->CreatePipeline(m_pipelineLayout, vertexInput, shaderStages, pipelineConfig.Options, attachmentColorBlends, renderPass, subpass);
}

void VulkanPipeline::Release() {
    if (m_device) {
        m_device->DestroyPipeline(m_pipeline);
        m_device->DestroyShaderModule(m_fragmentShaderModule);
        m_device->DestroyShaderModule(m_geometryShaderModule);
        m_device->DestroyShaderModule(m_vertexShaderModule);
        m_device->DestroyPipelineLayout(m_pipelineLayout);
    }

    m_device               = nullptr;
    m_pipeline             = VK_NULL_HANDLE;
    m_vertexShaderModule   = VK_NULL_HANDLE;
    m_geometryShaderModule = VK_NULL_HANDLE;
    m_fragmentShaderModule = VK_NULL_HANDLE;
    m_pipelineLayout       = VK_NULL_HANDLE;
}

void VulkanPipeline::Swap(VulkanPipeline &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_pipeline, other.m_pipeline);
    std::swap(m_vertexShaderModule, other.m_vertexShaderModule);
    std::swap(m_geometryShaderModule, other.m_geometryShaderModule);
    std::swap(m_fragmentShaderModule, other.m_fragmentShaderModule);
    std::swap(m_pipelineLayout, other.m_pipelineLayout);
}
