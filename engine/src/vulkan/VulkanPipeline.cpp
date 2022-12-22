//
// Created by andyroiiid on 12/22/2022.
//

#include "vulkan/VulkanPipeline.h"

#include "vulkan/VulkanDevice.h"

VulkanPipeline::VulkanPipeline(
        VulkanDevice &device,
        const std::initializer_list<vk::DescriptorSetLayout> &descriptorSetLayouts,
        const std::initializer_list<vk::PushConstantRange> &pushConstantRanges,
        const vk::PipelineVertexInputStateCreateInfo *vertexInput,
        const std::initializer_list<vk::PipelineShaderStageCreateInfo> &shaderStages,
        const VulkanPipelineOptions &options,
        const std::initializer_list<vk::PipelineColorBlendAttachmentState> &attachmentColorBlends,
        vk::RenderPass renderPass,
        uint32_t subpass
) : m_device(&device) {
    m_pipelineLayout = m_device->CreatePipelineLayout(
            descriptorSetLayouts,
            pushConstantRanges
    );

    m_pipeline = m_device->CreatePipeline(
            m_pipelineLayout,
            vertexInput,
            shaderStages,
            options,
            attachmentColorBlends,
            renderPass,
            subpass
    );
}

void VulkanPipeline::Release() {
    if (m_device) {
        m_device->DestroyPipeline(m_pipeline);
        m_device->DestroyPipelineLayout(m_pipelineLayout);
    }

    m_device = nullptr;
    m_pipeline = VK_NULL_HANDLE;
    m_pipelineLayout = VK_NULL_HANDLE;
}

void VulkanPipeline::Swap(VulkanPipeline &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_pipeline, other.m_pipeline);
    std::swap(m_pipelineLayout, other.m_pipelineLayout);
}
