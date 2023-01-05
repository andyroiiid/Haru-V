//
// Created by andyroiiid on 12/24/2022.
//

#include "gfx/DeferredFramebuffer.h"

DeferredFramebuffer::DeferredFramebuffer(
    VulkanBase                                    *device,
    vk::RenderPass                                 renderPass,
    vk::DescriptorSetLayout                        textureSetLayout,
    vk::Sampler                                    sampler,
    const vk::Extent2D                            &extent,
    const vk::ArrayProxyNoTemporaries<vk::Format> &colorFormats
)
    : m_device(device) {
    CreateAttachments(extent, colorFormats);
    CreateAttachmentViews(colorFormats);

    std::vector<vk::ImageView> attachmentViews = m_colorAttachmentViews;
    attachmentViews.push_back(m_depthAttachmentView);

    m_framebuffer = m_device->CreateFramebuffer(renderPass, attachmentViews, extent);

    m_textureSet = m_device->AllocateDescriptorSet(textureSetLayout);

    for (int i = 0; i < m_colorAttachmentViews.size(); i++) {
        m_device->WriteCombinedImageSamplerToDescriptorSet(
            sampler, //
            m_colorAttachmentViews[i],
            m_textureSet,
            i
        );
    }
}

void DeferredFramebuffer::CreateAttachments(const vk::Extent2D &extent, const vk::ArrayProxyNoTemporaries<vk::Format> &colorFormats) {
    for (const vk::Format &colorFormat: colorFormats) {
        VulkanImage attachment = m_device->CreateImage(
            colorFormat,
            extent,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
            0,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        );
        m_colorAttachments.push_back(std::move(attachment));
    }
    m_depthAttachment = m_device->CreateImage(
        vk::Format::eD32Sfloat, //
        extent,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        0,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    );
}

void DeferredFramebuffer::CreateAttachmentViews(const vk::ArrayProxyNoTemporaries<vk::Format> &colorFormats) {
    uint32_t i = 0;
    for (const vk::Format &colorFormat: colorFormats) {
        const vk::ImageView colorAttachmentView = m_device->CreateImageView(
            m_colorAttachments[i++].Get(), //
            colorFormat,
            vk::ImageAspectFlagBits::eColor
        );
        m_colorAttachmentViews.push_back(colorAttachmentView);
    }
    m_depthAttachmentView = m_device->CreateImageView(
        m_depthAttachment.Get(), //
        vk::Format::eD32Sfloat,
        vk::ImageAspectFlagBits::eDepth
    );
}

void DeferredFramebuffer::Release() {
    if (m_device) {
        m_device->FreeDescriptorSet(m_textureSet);
        m_device->DestroyFramebuffer(m_framebuffer);
        m_device->DestroyImageView(m_depthAttachmentView);
        for (const vk::ImageView &imageView: m_colorAttachmentViews) {
            m_device->DestroyImageView(imageView);
        }
    }

    m_device = nullptr;
    m_colorAttachments.clear();
    m_depthAttachment = {};
    m_colorAttachmentViews.clear();
    m_depthAttachmentView = VK_NULL_HANDLE;
    m_framebuffer         = VK_NULL_HANDLE;
    m_textureSet          = VK_NULL_HANDLE;
}

void DeferredFramebuffer::Swap(DeferredFramebuffer &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_colorAttachments, other.m_colorAttachments);
    std::swap(m_depthAttachment, other.m_depthAttachment);
    std::swap(m_colorAttachmentViews, other.m_colorAttachmentViews);
    std::swap(m_depthAttachmentView, other.m_depthAttachmentView);
    std::swap(m_framebuffer, other.m_framebuffer);
    std::swap(m_textureSet, other.m_textureSet);
}
