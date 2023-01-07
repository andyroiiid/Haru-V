//
// Created by andyroiiid on 1/6/2023.
//
#include "gfx/PostProcessingFramebuffer.h"

PostProcessingFramebuffer::PostProcessingFramebuffer(
    VulkanBase             *device,
    vk::RenderPass          renderPass,
    vk::DescriptorSetLayout textureSetLayout,
    vk::Sampler             sampler,
    const vk::Extent2D     &extent,
    vk::Format              format
)
    : m_device(device)
    , m_format(format) {
    CreateAttachments(extent);
    CreateAttachmentViews();

    m_framebuffer = m_device->CreateFramebuffer(renderPass, m_colorAttachmentView, extent);

    m_textureSet = m_device->AllocateDescriptorSet(textureSetLayout);

    m_device->WriteCombinedImageSamplerToDescriptorSet(
        sampler, //
        m_colorAttachmentView,
        m_textureSet,
        0
    );
}

void PostProcessingFramebuffer::CreateAttachments(const vk::Extent2D &extent) {
    m_colorAttachment = m_device->CreateImage(
        m_format,
        extent,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
        0,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    );
}

void PostProcessingFramebuffer::CreateAttachmentViews() {
    m_colorAttachmentView = m_device->CreateImageView(
        m_colorAttachment.Get(), //
        m_format,
        vk::ImageAspectFlagBits::eColor
    );
}

void PostProcessingFramebuffer::Release() {
    if (m_device) {
        m_device->FreeDescriptorSet(m_textureSet);
        m_device->DestroyFramebuffer(m_framebuffer);
        m_device->DestroyImageView(m_colorAttachmentView);
    }

    m_device              = nullptr;
    m_colorAttachment     = {};
    m_colorAttachmentView = VK_NULL_HANDLE;
    m_framebuffer         = VK_NULL_HANDLE;
    m_textureSet          = VK_NULL_HANDLE;
}

void PostProcessingFramebuffer::Swap(PostProcessingFramebuffer &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_colorAttachment, other.m_colorAttachment);
    std::swap(m_colorAttachmentView, other.m_colorAttachmentView);
    std::swap(m_framebuffer, other.m_framebuffer);
    std::swap(m_textureSet, other.m_textureSet);
}
