//
// Created by andyroiiid on 12/24/2022.
//

#pragma once

#include "vulkan/VulkanBase.h"

#include "DeferredFramebuffer.h"

class DeferredContext {
public:
    DeferredContext() = default;

    explicit DeferredContext(VulkanBase &device);

    ~DeferredContext() {
        Release();
    }

    DeferredContext(const DeferredContext &) = delete;

    DeferredContext &operator=(const DeferredContext &) = delete;

    DeferredContext(DeferredContext &&other) noexcept {
        Swap(other);
    }

    DeferredContext &operator=(DeferredContext &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(DeferredContext &other) noexcept;

    [[nodiscard]] const vk::RenderPass &GetRenderPass() const { return m_renderPass; }

    [[nodiscard]] const vk::DescriptorSetLayout &GetTextureSetLayout() const { return m_textureSetLayout; }

    [[nodiscard]] const vk::Extent2D &GetExtent() const { return m_extent; }

    [[nodiscard]] const vk::Framebuffer &GetFramebuffer(uint32_t bufferingIndex) { return m_framebuffers[bufferingIndex].GetFramebuffer(); }

    [[nodiscard]] const vk::DescriptorSet &GetTextureSet(uint32_t bufferingIndex) { return m_framebuffers[bufferingIndex].GetTextureSet(); }

    void CheckFramebuffersOutOfDate();

private:
    void CreateRenderPass();

    void CreateFramebuffers();

    VulkanBase *m_device = nullptr;

    vk::RenderPass m_renderPass;
    vk::DescriptorSetLayout m_textureSetLayout;
    vk::Sampler m_sampler;

    vk::Extent2D m_extent;
    std::vector<DeferredFramebuffer> m_framebuffers;
};
