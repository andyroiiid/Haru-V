//
// Created by andyroiiid on 12/30/2022.
//

#include "gfx/ShadowContext.h"

ShadowContext::ShadowContext(VulkanBase &device)
    : m_device(&device) {
    CreateRenderPass();
    CreateFramebuffers();
}

void ShadowContext::CreateRenderPass() {
    VulkanRenderPassOptions options;
    options.ShaderReadsDepth = true;

    m_renderPass = m_device->CreateRenderPass(
        {}, //
        vk::Format::eD32Sfloat,
        options
    );

    vk::DescriptorSetLayoutBinding bindings[]{
        {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_textureSetLayout = m_device->CreateDescriptorSetLayout(bindings);

    m_sampler = m_device->CreateSampler(
        vk::Filter::eLinear, //
        vk::SamplerAddressMode::eClampToEdge,
        VK_TRUE,
        vk::CompareOp::eGreaterOrEqual
    );
}

void ShadowContext::CreateFramebuffers() {
    m_extent                  = vk::Extent2D{2048, 2048};
    const size_t numBuffering = m_device->GetNumBuffering();

    const vk::Rect2D renderArea{
        {0, 0},
        m_extent
    };
    static vk::ClearValue const clearValues[]{{vk::ClearDepthStencilValue{1.0f, 0}}};

    vk::RenderPassBeginInfo beginInfo(
        m_renderPass, //
        {},
        renderArea,
        clearValues
    );
    for (int i = 0; i < numBuffering; i++) {
        const ShadowMap &framebuffer = m_framebuffers.emplace_back(
            m_device, //
            m_renderPass,
            m_textureSetLayout,
            m_sampler,
            m_extent
        );
        beginInfo.framebuffer = framebuffer.GetFramebuffer();
        m_renderPassBeginInfos.push_back(beginInfo);
    }
}

void ShadowContext::CleanupFramebuffers() {
    m_extent = vk::Extent2D{};
    m_framebuffers.clear();
    m_renderPassBeginInfos.clear();
}

void ShadowContext::Release() {
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

void ShadowContext::Swap(ShadowContext &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_renderPass, other.m_renderPass);
    std::swap(m_textureSetLayout, other.m_textureSetLayout);
    std::swap(m_sampler, other.m_sampler);
    std::swap(m_extent, other.m_extent);
    std::swap(m_framebuffers, other.m_framebuffers);
    std::swap(m_renderPassBeginInfos, other.m_renderPassBeginInfos);
}
