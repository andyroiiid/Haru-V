//
// Created by andyroiiid on 12/30/2022.
//

#include "gfx/ShadowMaps.h"

ShadowMap::ShadowMap(
    VulkanBase             *device,
    vk::RenderPass          renderPass,
    vk::DescriptorSetLayout textureSetLayout,
    vk::Sampler             sampler,
    const vk::Extent2D     &extent
)
    : m_device(device) {
    CreateAttachment(extent);
    CreateAttachmentView();

    m_framebuffer = m_device->CreateFramebuffer(
        renderPass, //
        m_depthAttachmentView,
        extent,
        4
    );

    m_textureSet = m_device->AllocateDescriptorSet(textureSetLayout);

    m_device->WriteCombinedImageSamplerToDescriptorSet(
        sampler, //
        m_depthAttachmentView,
        m_textureSet,
        0
    );
}

void ShadowMap::CreateAttachment(const vk::Extent2D &extent) {
    m_depthAttachment = m_device->CreateImage(
        vk::Format::eD32Sfloat,
        extent,
        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
        0,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        4
    );
}

void ShadowMap::CreateAttachmentView() {
    m_depthAttachmentView = m_device->CreateImageView(
        m_depthAttachment.Get(), //
        vk::Format::eD32Sfloat,
        vk::ImageAspectFlagBits::eDepth,
        4
    );
}

void ShadowMap::Release() {
    if (m_device) {
        m_device->FreeDescriptorSet(m_textureSet);
        m_device->DestroyFramebuffer(m_framebuffer);
        m_device->DestroyImageView(m_depthAttachmentView);
    }

    m_device              = nullptr;
    m_depthAttachment     = {};
    m_depthAttachmentView = VK_NULL_HANDLE;
    m_framebuffer         = VK_NULL_HANDLE;
    m_textureSet          = VK_NULL_HANDLE;
}

void ShadowMap::Swap(ShadowMap &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_depthAttachment, other.m_depthAttachment);
    std::swap(m_depthAttachmentView, other.m_depthAttachmentView);
    std::swap(m_framebuffer, other.m_framebuffer);
    std::swap(m_textureSet, other.m_textureSet);
}
