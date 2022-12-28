//
// Created by andyroiiid on 12/24/2022.
//

#pragma once

#include <vulkan/VulkanBase.h>

class DeferredFramebuffer {
public:
    DeferredFramebuffer() = default;

    DeferredFramebuffer(
            VulkanBase *device,
            vk::RenderPass renderPass,
            vk::DescriptorSetLayout textureSetLayout,
            vk::Sampler sampler,
            const vk::Extent2D &extent,
            const vk::ArrayProxyNoTemporaries<vk::Format> &colorFormats
    );

    ~DeferredFramebuffer() {
        Release();
    }

    DeferredFramebuffer(const DeferredFramebuffer &) = delete;

    DeferredFramebuffer &operator=(const DeferredFramebuffer &) = delete;

    DeferredFramebuffer(DeferredFramebuffer &&other) noexcept {
        Swap(other);
    }

    DeferredFramebuffer &operator=(DeferredFramebuffer &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(DeferredFramebuffer &other) noexcept;

    [[nodiscard]] const vk::Framebuffer &GetFramebuffer() const { return m_framebuffer; }

    [[nodiscard]] const vk::DescriptorSet &GetTextureSet() const { return m_textureSet; }

private:
    void CreateAttachments(const vk::Extent2D &extent, const vk::ArrayProxyNoTemporaries<vk::Format> &colorFormats);

    void CreateAttachmentViews(const vk::ArrayProxyNoTemporaries<vk::Format> &colorFormats);

    VulkanBase *m_device = nullptr;

    std::vector<VulkanImage> m_colorAttachments;
    std::vector<vk::ImageView> m_colorAttachmentViews;
    VulkanImage m_depthAttachment;
    vk::ImageView m_depthAttachmentView;
    vk::Framebuffer m_framebuffer;
    vk::DescriptorSet m_textureSet;
};
