//
// Created by andyroiiid on 1/5/2023.
//
#include "gfx/ForwardFramebuffer.h"

ForwardFramebuffer::ForwardFramebuffer(
    VulkanBase             *device,
    vk::RenderPass          renderPass,
    vk::DescriptorSetLayout textureSetLayout,
    vk::Sampler             sampler,
    const vk::Extent2D     &extent,
    vk::ImageView           depthImageView
)
    : m_device(device) {
    CreateAttachments(extent);
    CreateAttachmentViews();

    std::vector<vk::ImageView> attachmentViews{m_colorAttachmentView, depthImageView};

    m_framebuffer = m_device->CreateFramebuffer(renderPass, attachmentViews, extent);

    m_textureSet = m_device->AllocateDescriptorSet(textureSetLayout);

    m_device->WriteCombinedImageSamplerToDescriptorSet(
        sampler, //
        m_colorAttachmentView,
        m_textureSet,
        0
    );
}

void ForwardFramebuffer::CreateAttachments(const vk::Extent2D &extent) {
    m_colorAttachment = m_device->CreateImage(
        vk::Format::eR32G32B32A32Sfloat,
        extent,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
        0,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    );
}

void ForwardFramebuffer::CreateAttachmentViews() {
    m_colorAttachmentView = m_device->CreateImageView(
        m_colorAttachment.Get(), //
        vk::Format::eR32G32B32A32Sfloat,
        vk::ImageAspectFlagBits::eColor
    );
}

void ForwardFramebuffer::Release() {
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

void ForwardFramebuffer::Swap(ForwardFramebuffer &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_colorAttachment, other.m_colorAttachment);
    std::swap(m_colorAttachmentView, other.m_colorAttachmentView);
    std::swap(m_framebuffer, other.m_framebuffer);
    std::swap(m_textureSet, other.m_textureSet);
}
