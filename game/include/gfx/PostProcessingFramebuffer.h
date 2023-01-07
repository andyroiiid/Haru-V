//
// Created by andyroiiid on 1/6/2023.
//

#pragma once

#include <vulkan/VulkanBase.h>

class PostProcessingFramebuffer {
public:
    PostProcessingFramebuffer() = default;

    PostProcessingFramebuffer(
        VulkanBase             *device,
        vk::RenderPass          renderPass,
        vk::DescriptorSetLayout textureSetLayout,
        vk::Sampler             sampler,
        const vk::Extent2D     &extent,
        vk::Format              format = vk::Format::eB8G8R8A8Unorm
    );

    ~PostProcessingFramebuffer() { Release(); }

    PostProcessingFramebuffer(const PostProcessingFramebuffer &) = delete;

    PostProcessingFramebuffer &operator=(const PostProcessingFramebuffer &) = delete;

    PostProcessingFramebuffer(PostProcessingFramebuffer &&other) noexcept { Swap(other); }

    PostProcessingFramebuffer &operator=(PostProcessingFramebuffer &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(PostProcessingFramebuffer &other) noexcept;

    [[nodiscard]] const vk::Framebuffer &GetFramebuffer() const { return m_framebuffer; }

    [[nodiscard]] const vk::DescriptorSet &GetTextureSet() const { return m_textureSet; }

private:
    void CreateAttachments(const vk::Extent2D &extent);

    void CreateAttachmentViews();

    VulkanBase *m_device = nullptr;

    vk::Format        m_format = vk::Format::eUndefined;
    VulkanImage       m_colorAttachment;
    vk::ImageView     m_colorAttachmentView;
    vk::Framebuffer   m_framebuffer;
    vk::DescriptorSet m_textureSet;
};
