//
// Created by andyroiiid on 1/6/2023.
//
#include "gfx/PostProcessingContext.h"

PostProcessingContext::PostProcessingContext(VulkanBase &device)
    : m_device(&device) {
    m_sampler = m_device->CreateSampler(vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge);

    CreateRenderPass();
    CreateFramebuffers();
}

void PostProcessingContext::CreateRenderPass() {
    m_renderPass = m_device->CreateRenderPass(
        {vk::Format::eR8G8B8A8Unorm}, //
        vk::Format::eUndefined
    );

    vk::DescriptorSetLayoutBinding bindings[]{
        {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_textureSetLayout = m_device->CreateDescriptorSetLayout(bindings);
}

void PostProcessingContext::CreateFramebuffers() {
    m_extent = m_device->GetSwapchainExtent();

    const size_t numBuffering = m_device->GetNumBuffering();

    const vk::Rect2D renderArea{
        {0, 0},
        m_extent
    };

    static const vk::ClearValue clearValues[]{
        {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
    };
    vk::RenderPassBeginInfo beginInfo(
        m_renderPass, //
        {},
        renderArea,
        clearValues
    );
    for (int i = 0; i < numBuffering; i++) {
        const PostProcessingFramebuffer &framebuffer = m_framebuffers.emplace_back(
            m_device, //
            m_renderPass,
            m_textureSetLayout,
            m_sampler,
            m_extent,
            vk::Format::eR8G8B8A8Unorm
        );
        beginInfo.framebuffer = framebuffer.GetFramebuffer();
        m_renderPassBeginInfo.push_back(beginInfo);
    }
}

void PostProcessingContext::CleanupFramebuffers() {
    m_extent = vk::Extent2D{};
    m_framebuffers.clear();
    m_renderPassBeginInfo.clear();
}

void PostProcessingContext::Release() {
    if (m_device) {
        m_device->DestroySampler(m_sampler);
        m_device->DestroyDescriptorSetLayout(m_textureSetLayout);
        m_device->DestroyRenderPass(m_renderPass);
    }

    m_device           = nullptr;
    m_renderPass       = VK_NULL_HANDLE;
    m_textureSetLayout = VK_NULL_HANDLE;
    m_sampler          = VK_NULL_HANDLE;
    CleanupFramebuffers();
}

void PostProcessingContext::Swap(PostProcessingContext &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_extent, other.m_extent);
    std::swap(m_sampler, other.m_sampler);
    std::swap(m_renderPass, other.m_renderPass);
    std::swap(m_textureSetLayout, other.m_textureSetLayout);
    std::swap(m_framebuffers, other.m_framebuffers);
    std::swap(m_renderPassBeginInfo, other.m_renderPassBeginInfo);
}

void PostProcessingContext::CheckFramebuffersOutOfDate() {
    if (m_device->GetSwapchainExtent() == m_extent) {
        return;
    }

    m_device->WaitIdle();
    CleanupFramebuffers();
    CreateFramebuffers();
}
