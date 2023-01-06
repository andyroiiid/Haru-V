//
// Created by andyroiiid on 12/24/2022.
//

#include "gfx/DeferredContext.h"

DeferredContext::DeferredContext(VulkanBase &device)
    : m_device(&device) {
    m_sampler = m_device->CreateSampler(vk::Filter::eNearest, vk::SamplerAddressMode::eClampToEdge);

    CreateRenderPass();
    CreateFramebuffers();
}

void DeferredContext::CreateRenderPass() {
    m_deferredRenderPass = m_device->CreateRenderPass(
        {vk::Format::eR32G32B32A32Sfloat, //
         vk::Format::eR32G32B32A32Sfloat,
         vk::Format::eR32G32B32A32Sfloat,
         vk::Format::eR32G32B32A32Sfloat},
        vk::Format::eD32Sfloat
    );

    vk::DescriptorSetLayoutBinding deferredBindings[]{
        {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
        {1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
        {2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
        {3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_deferredTextureSetLayout = m_device->CreateDescriptorSetLayout(deferredBindings);

    VulkanRenderPassOptions forwardRenderPassOptions;
    forwardRenderPassOptions.PreserveDepth = true;

    m_forwardRenderPass = m_device->CreateRenderPass(
        {vk::Format::eR32G32B32A32Sfloat}, //
        vk::Format::eD32Sfloat,
        forwardRenderPassOptions
    );

    vk::DescriptorSetLayoutBinding forwardBindings[]{
        {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_forwardTextureSetLayout = m_device->CreateDescriptorSetLayout(forwardBindings);
}

void DeferredContext::CreateFramebuffers() {
    m_extent = m_device->GetSwapchainExtent();

    const size_t numBuffering = m_device->GetNumBuffering();

    const vk::Rect2D renderArea{
        {0, 0},
        m_extent
    };

    {
        static const vk::ClearValue clearValues[]{
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
            {vk::ClearDepthStencilValue{1.0f, 0}}};
        static vk::Format colorFormats[] = {
            vk::Format::eR32G32B32A32Sfloat, //
            vk::Format::eR32G32B32A32Sfloat,
            vk::Format::eR32G32B32A32Sfloat,
            vk::Format::eR32G32B32A32Sfloat};
        vk::RenderPassBeginInfo beginInfo(
            m_deferredRenderPass, //
            {},
            renderArea,
            clearValues
        );
        for (int i = 0; i < numBuffering; i++) {
            const DeferredFramebuffer &framebuffer = m_deferredFramebuffers.emplace_back(
                m_device, //
                m_deferredRenderPass,
                m_deferredTextureSetLayout,
                m_sampler,
                m_extent,
                colorFormats
            );
            beginInfo.framebuffer = framebuffer.GetFramebuffer();
            m_deferredRenderPassBeginInfo.push_back(beginInfo);
        }
    }

    {
        static const vk::ClearValue clearValues[]{
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}}},
        };
        vk::RenderPassBeginInfo beginInfo(
            m_forwardRenderPass, //
            {},
            renderArea,
            clearValues
        );
        for (int i = 0; i < numBuffering; i++) {
            const ForwardFramebuffer &framebuffer = m_forwardFramebuffers.emplace_back(
                m_device, //
                m_forwardRenderPass,
                m_forwardTextureSetLayout,
                m_sampler,
                m_extent,
                m_deferredFramebuffers[i].GetDepthAttachmentView()
            );
            beginInfo.framebuffer = framebuffer.GetFramebuffer();
            m_forwardRenderPassBeginInfo.push_back(beginInfo);
        }
    }
}

void DeferredContext::CleanupFramebuffers() {
    m_extent = vk::Extent2D{};
    m_deferredFramebuffers.clear();
    m_deferredRenderPassBeginInfo.clear();
    m_forwardFramebuffers.clear();
    m_forwardRenderPassBeginInfo.clear();
}

void DeferredContext::Release() {
    if (m_device) {
        m_device->DestroySampler(m_sampler);
        m_device->DestroyDescriptorSetLayout(m_deferredTextureSetLayout);
        m_device->DestroyRenderPass(m_deferredRenderPass);
        m_device->DestroyDescriptorSetLayout(m_forwardTextureSetLayout);
        m_device->DestroyRenderPass(m_forwardRenderPass);
    }

    m_device                   = nullptr;
    m_deferredRenderPass       = VK_NULL_HANDLE;
    m_deferredTextureSetLayout = VK_NULL_HANDLE;
    m_forwardRenderPass        = VK_NULL_HANDLE;
    m_forwardTextureSetLayout  = VK_NULL_HANDLE;
    m_sampler                  = VK_NULL_HANDLE;
    CleanupFramebuffers();
}

void DeferredContext::Swap(DeferredContext &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_deferredRenderPass, other.m_deferredRenderPass);
    std::swap(m_deferredTextureSetLayout, other.m_deferredTextureSetLayout);
    std::swap(m_forwardRenderPass, other.m_forwardRenderPass);
    std::swap(m_forwardTextureSetLayout, other.m_forwardTextureSetLayout);
    std::swap(m_sampler, other.m_sampler);
    std::swap(m_extent, other.m_extent);
    std::swap(m_deferredFramebuffers, other.m_deferredFramebuffers);
    std::swap(m_deferredRenderPassBeginInfo, other.m_deferredRenderPassBeginInfo);
    std::swap(m_forwardFramebuffers, other.m_forwardFramebuffers);
    std::swap(m_forwardRenderPassBeginInfo, other.m_forwardRenderPassBeginInfo);
}

void DeferredContext::CheckFramebuffersOutOfDate() {
    if (m_device->GetSwapchainExtent() == m_extent) {
        return;
    }

    m_device->WaitIdle();
    CleanupFramebuffers();
    CreateFramebuffers();
}
