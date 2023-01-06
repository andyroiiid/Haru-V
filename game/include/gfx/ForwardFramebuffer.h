//
// Created by andyroiiid on 1/5/2023.
//

#pragma once

#include <vulkan/VulkanBase.h>

class ForwardFramebuffer {
public:
    ForwardFramebuffer() = default;

    ForwardFramebuffer(
        VulkanBase             *device,
        vk::RenderPass          renderPass,
        vk::DescriptorSetLayout textureSetLayout,
        vk::Sampler             sampler,
        const vk::Extent2D     &extent,
        vk::ImageView           depthImageView
    );

    ~ForwardFramebuffer() { Release(); }

    ForwardFramebuffer(const ForwardFramebuffer &) = delete;

    ForwardFramebuffer &operator=(const ForwardFramebuffer &) = delete;

    ForwardFramebuffer(ForwardFramebuffer &&other) noexcept { Swap(other); }

    ForwardFramebuffer &operator=(ForwardFramebuffer &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(ForwardFramebuffer &other) noexcept;

    [[nodiscard]] const vk::Framebuffer &GetFramebuffer() const { return m_framebuffer; }

    [[nodiscard]] const vk::DescriptorSet &GetTextureSet() const { return m_textureSet; }

private:
    void CreateAttachments(const vk::Extent2D &extent);

    void CreateAttachmentViews();

    VulkanBase *m_device = nullptr;

    VulkanImage       m_colorAttachment;
    vk::ImageView     m_colorAttachmentView;
    vk::Framebuffer   m_framebuffer;
    vk::DescriptorSet m_textureSet;
};
