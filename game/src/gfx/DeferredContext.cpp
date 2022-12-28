//
// Created by andyroiiid on 12/24/2022.
//

#include "gfx/DeferredContext.h"

DeferredContext::DeferredContext(VulkanBase &device)
        : m_device(&device) {
    CreateRenderPass();
    CreateFramebuffers();
}

void DeferredContext::CreateRenderPass() {
    m_renderPass = m_device->CreateRenderPass(
            {
                    vk::Format::eR32G32B32A32Sfloat,
                    vk::Format::eR32G32B32A32Sfloat,
                    vk::Format::eR32G32B32A32Sfloat,
                    vk::Format::eR32G32B32A32Sfloat
            },
            vk::Format::eD32Sfloat
    );

    vk::DescriptorSetLayoutBinding bindings[]{
            {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
            {1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
            {2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
            {3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_textureSetLayout = m_device->CreateDescriptorSetLayout(bindings);

    m_sampler = m_device->CreateSampler(vk::Filter::eNearest, vk::SamplerAddressMode::eClampToEdge);
}

void DeferredContext::CreateFramebuffers() {
    static vk::ClearValue const clearValues[]{
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
            {vk::ClearDepthStencilValue{1.0f, 0}}
    };
    m_extent = m_device->GetSwapchainExtent();
    const size_t numBuffering = m_device->GetNumBuffering();
    static vk::Format colorFormats[] = {
            vk::Format::eR32G32B32A32Sfloat,
            vk::Format::eR32G32B32A32Sfloat,
            vk::Format::eR32G32B32A32Sfloat,
            vk::Format::eR32G32B32A32Sfloat
    };
    vk::RenderPassBeginInfo beginInfo(
            m_renderPass,
            {},
            {{0, 0}, m_extent},
            clearValues
    );
    for (int i = 0; i < numBuffering; i++) {
        const DeferredFramebuffer &framebuffer = m_framebuffers.emplace_back(
                m_device,
                m_renderPass,
                m_textureSetLayout,
                m_sampler,
                m_extent,
                colorFormats
        );
        beginInfo.framebuffer = framebuffer.GetFramebuffer();
        m_renderPassBeginInfos.push_back(beginInfo);
    }
}

void DeferredContext::CleanupFramebuffers() {
    m_extent = vk::Extent2D{};
    m_framebuffers.clear();
    m_renderPassBeginInfos.clear();
}

void DeferredContext::Release() {
    if (m_device) {
        m_device->DestroySampler(m_sampler);
        m_device->DestroyDescriptorSetLayout(m_textureSetLayout);
        m_device->DestroyRenderPass(m_renderPass);
    }

    m_device = nullptr;
    m_renderPass = VK_NULL_HANDLE;
    m_textureSetLayout = VK_NULL_HANDLE;
    m_sampler = VK_NULL_HANDLE;
    CleanupFramebuffers();
}

void DeferredContext::Swap(DeferredContext &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_renderPass, other.m_renderPass);
    std::swap(m_textureSetLayout, other.m_textureSetLayout);
    std::swap(m_sampler, other.m_sampler);
    std::swap(m_extent, other.m_extent);
    std::swap(m_framebuffers, other.m_framebuffers);
    std::swap(m_renderPassBeginInfos, other.m_renderPassBeginInfos);
}

void DeferredContext::CheckFramebuffersOutOfDate() {
    if (m_device->GetSwapchainExtent() == m_extent) {
        return;
    }

    m_device->WaitIdle();
    CleanupFramebuffers();
    CreateFramebuffers();
}
