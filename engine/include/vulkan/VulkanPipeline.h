//
// Created by andyroiiid on 12/22/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>

class VulkanDevice;

struct VulkanPipelineOptions;

class VulkanPipeline {
public:
    VulkanPipeline() = default;

    VulkanPipeline(
            VulkanDevice &device,
            const std::initializer_list<vk::DescriptorSetLayout> &descriptorSetLayouts,
            const std::initializer_list<vk::PushConstantRange> &pushConstantRanges,
            const vk::PipelineVertexInputStateCreateInfo *vertexInput,
            const std::string &shaderConfigFile,
            const std::initializer_list<vk::PipelineColorBlendAttachmentState> &attachmentColorBlends,
            vk::RenderPass renderPass,
            uint32_t subpass
    );

    ~VulkanPipeline() {
        Release();
    }

    VulkanPipeline(const VulkanPipeline &) = delete;

    VulkanPipeline &operator=(const VulkanPipeline &) = delete;

    VulkanPipeline(VulkanPipeline &&other) noexcept {
        Swap(other);
    }

    VulkanPipeline &operator=(VulkanPipeline &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(VulkanPipeline &other) noexcept;

    [[nodiscard]] const vk::PipelineLayout &GetLayout() const { return m_pipelineLayout; }

    [[nodiscard]] const vk::Pipeline &Get() const { return m_pipeline; }

private:
    VulkanDevice *m_device = nullptr;

    vk::PipelineLayout m_pipelineLayout;
    vk::ShaderModule m_vertexShaderModule;
    vk::ShaderModule m_fragmentShaderModule;
    vk::Pipeline m_pipeline;
};
