//
// Created by andyroiiid on 12/30/2022.
//

#pragma once

#include <vulkan/VulkanBase.h>

class ShadowMap {
public:
    ShadowMap() = default;

    ShadowMap(
            VulkanBase *device,
            vk::RenderPass renderPass,
            vk::DescriptorSetLayout textureSetLayout,
            vk::Sampler sampler,
            const vk::Extent2D &extent
    );

    ~ShadowMap() {
        Release();
    }

    ShadowMap(const ShadowMap &) = delete;

    ShadowMap &operator=(const ShadowMap &) = delete;

    ShadowMap(ShadowMap &&other) noexcept {
        Swap(other);
    }

    ShadowMap &operator=(ShadowMap &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(ShadowMap &other) noexcept;

    [[nodiscard]] const vk::Framebuffer &GetFramebuffer() const { return m_framebuffer; }

    [[nodiscard]] const vk::DescriptorSet &GetTextureSet() const { return m_textureSet; }

private:
    void CreateAttachment(const vk::Extent2D &extent);

    void CreateAttachmentView();

    VulkanBase *m_device = nullptr;

    VulkanImage m_depthAttachment;
    vk::ImageView m_depthAttachmentView;
    vk::Framebuffer m_framebuffer;
    vk::DescriptorSet m_textureSet;
};
