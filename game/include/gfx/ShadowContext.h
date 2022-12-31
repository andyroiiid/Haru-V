//
// Created by andyroiiid on 12/30/2022.
//

#pragma once

#include <vulkan/VulkanBase.h>

#include "gfx/ShadowMaps.h"

class ShadowContext {
public:
    ShadowContext() = default;

    explicit ShadowContext(VulkanBase &device);

    ~ShadowContext() {
        Release();
    }

    ShadowContext(const ShadowContext &) = delete;

    ShadowContext &operator=(const ShadowContext &) = delete;

    ShadowContext(ShadowContext &&other) noexcept {
        Swap(other);
    }

    ShadowContext &operator=(ShadowContext &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(ShadowContext &other) noexcept;

    [[nodiscard]] const vk::RenderPass &GetRenderPass() const { return m_renderPass; }

    [[nodiscard]] const vk::DescriptorSetLayout &GetTextureSetLayout() const { return m_textureSetLayout; }

    [[nodiscard]] const vk::Extent2D &GetExtent() const { return m_extent; }

    [[nodiscard]] const vk::DescriptorSet &GetTextureSet(uint32_t bufferingIndex) const {
        return m_framebuffers[bufferingIndex].GetTextureSet();
    }

    [[nodiscard]] const vk::RenderPassBeginInfo *GetRenderPassBeginInfo(uint32_t bufferingIndex) const {
        return &m_renderPassBeginInfos[bufferingIndex];
    }

private:
    void CreateRenderPass();

    void CreateFramebuffers();

    void CleanupFramebuffers();

    VulkanBase *m_device = nullptr;

    vk::RenderPass m_renderPass;
    vk::DescriptorSetLayout m_textureSetLayout;
    vk::Sampler m_sampler;

    vk::Extent2D m_extent;
    std::vector<ShadowMap> m_framebuffers;
    std::vector<vk::RenderPassBeginInfo> m_renderPassBeginInfos;
};
