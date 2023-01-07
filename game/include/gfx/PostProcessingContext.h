//
// Created by andyroiiid on 1/6/2023.
//

#pragma once

#include <vulkan/VulkanBase.h>

#include "gfx/PostProcessingFramebuffer.h"

class PostProcessingContext {
public:
    PostProcessingContext() = default;

    explicit PostProcessingContext(VulkanBase &device);

    ~PostProcessingContext() { Release(); }

    PostProcessingContext(const PostProcessingContext &) = delete;

    PostProcessingContext &operator=(const PostProcessingContext &) = delete;

    PostProcessingContext(PostProcessingContext &&other) noexcept { Swap(other); }

    PostProcessingContext &operator=(PostProcessingContext &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(PostProcessingContext &other) noexcept;

    [[nodiscard]] const vk::Extent2D &GetExtent() const { return m_extent; }

    [[nodiscard]] const vk::RenderPass &GetRenderPass() const { return m_renderPass; }

    [[nodiscard]] const vk::DescriptorSetLayout &GetTextureSetLayout() const { return m_textureSetLayout; }

    [[nodiscard]] const vk::DescriptorSet &GetTextureSet(uint32_t bufferingIndex) const { return m_framebuffers[bufferingIndex].GetTextureSet(); }

    [[nodiscard]] const vk::RenderPassBeginInfo *GetRenderPassBeginInfo(uint32_t bufferingIndex) const {
        return &m_renderPassBeginInfo[bufferingIndex];
    }

    void CheckFramebuffersOutOfDate();

private:
    void CreateRenderPass();

    void CreateFramebuffers();

    void CleanupFramebuffers();

    VulkanBase *m_device = nullptr;

    vk::Extent2D m_extent;
    vk::Sampler  m_sampler;

    vk::RenderPass          m_renderPass;
    vk::DescriptorSetLayout m_textureSetLayout;

    std::vector<PostProcessingFramebuffer> m_framebuffers;
    std::vector<vk::RenderPassBeginInfo>   m_renderPassBeginInfo;
};
