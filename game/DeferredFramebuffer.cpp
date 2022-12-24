//
// Created by andyroiiid on 12/24/2022.
//

#include "DeferredFramebuffer.h"

DeferredFramebuffer::DeferredFramebuffer(
        VulkanBase *device,
        vk::RenderPass renderPass,
        vk::DescriptorSetLayout textureSetLayout,
        vk::Sampler sampler,
        const vk::Extent2D &extent
) : m_device(device) {
    CreateAttachments(extent);
    CreateAttachmentViews();

    // framebuffer
    m_framebuffer = m_device->CreateFramebuffer(
            renderPass,
            {
                    m_worldPositionAttachmentView,
                    m_worldNormalAttachmentView,
                    m_diffuseAttachmentView,
                    m_depthAttachmentView
            },
            extent
    );

    // deferred texture descriptor set
    m_textureSet = m_device->AllocateDescriptorSet(textureSetLayout);

    // bind textures to deferred texture descriptor set
    m_device->WriteCombinedImageSamplerToDescriptorSet(sampler, m_worldPositionAttachmentView, m_textureSet, 0);
    m_device->WriteCombinedImageSamplerToDescriptorSet(sampler, m_worldNormalAttachmentView, m_textureSet, 1);
    m_device->WriteCombinedImageSamplerToDescriptorSet(sampler, m_diffuseAttachmentView, m_textureSet, 2);
}

void DeferredFramebuffer::CreateAttachments(const vk::Extent2D &extent) {
    m_worldPositionAttachment = m_device->CreateImage(
            vk::Format::eR32G32B32A32Sfloat,
            extent,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
            0,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    );
    m_worldNormalAttachment = m_device->CreateImage(
            vk::Format::eR32G32B32A32Sfloat,
            extent,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
            0,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    );
    m_diffuseAttachment = m_device->CreateImage(
            vk::Format::eR8G8B8A8Unorm,
            extent,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
            0,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    );
    m_depthAttachment = m_device->CreateImage(
            vk::Format::eD32Sfloat,
            extent,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            0,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    );
}

void DeferredFramebuffer::CreateAttachmentViews() {
    m_worldPositionAttachmentView = m_device->CreateImageView(
            m_worldPositionAttachment.Get(),
            vk::Format::eR32G32B32A32Sfloat,
            vk::ImageAspectFlagBits::eColor
    );
    m_worldNormalAttachmentView = m_device->CreateImageView(
            m_worldNormalAttachment.Get(),
            vk::Format::eR32G32B32A32Sfloat,
            vk::ImageAspectFlagBits::eColor
    );
    m_diffuseAttachmentView = m_device->CreateImageView(
            m_diffuseAttachment.Get(),
            vk::Format::eR8G8B8A8Unorm,
            vk::ImageAspectFlagBits::eColor
    );
    m_depthAttachmentView = m_device->CreateImageView(
            m_depthAttachment.Get(),
            vk::Format::eD32Sfloat,
            vk::ImageAspectFlagBits::eDepth
    );
}

void DeferredFramebuffer::Release() {
    if (m_device) {
        m_device->FreeDescriptorSet(m_textureSet);
        m_device->DestroyFramebuffer(m_framebuffer);
        m_device->DestroyImageView(m_depthAttachmentView);
        m_device->DestroyImageView(m_diffuseAttachmentView);
        m_device->DestroyImageView(m_worldNormalAttachmentView);
        m_device->DestroyImageView(m_worldPositionAttachmentView);
    }

    m_device = nullptr;
    m_worldPositionAttachment = {};
    m_worldNormalAttachment = {};
    m_diffuseAttachment = {};
    m_depthAttachment = {};
    m_worldPositionAttachmentView = VK_NULL_HANDLE;
    m_worldNormalAttachmentView = VK_NULL_HANDLE;
    m_diffuseAttachmentView = VK_NULL_HANDLE;
    m_depthAttachmentView = VK_NULL_HANDLE;
    m_framebuffer = VK_NULL_HANDLE;
    m_textureSet = VK_NULL_HANDLE;
}

void DeferredFramebuffer::Swap(DeferredFramebuffer &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_worldPositionAttachment, other.m_worldPositionAttachment);
    std::swap(m_worldNormalAttachment, other.m_worldNormalAttachment);
    std::swap(m_diffuseAttachment, other.m_diffuseAttachment);
    std::swap(m_depthAttachment, other.m_depthAttachment);
    std::swap(m_worldPositionAttachmentView, other.m_worldPositionAttachmentView);
    std::swap(m_worldNormalAttachmentView, other.m_worldNormalAttachmentView);
    std::swap(m_diffuseAttachmentView, other.m_diffuseAttachmentView);
    std::swap(m_depthAttachmentView, other.m_depthAttachmentView);
    std::swap(m_framebuffer, other.m_framebuffer);
    std::swap(m_textureSet, other.m_textureSet);
}
