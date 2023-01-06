//
// Created by andyroiiid on 12/24/2022.
//

#pragma once

#include <vulkan/VulkanBase.h>

#include "gfx/DeferredFramebuffer.h"
#include "gfx/ForwardFramebuffer.h"

class DeferredContext {
public:
    DeferredContext() = default;

    explicit DeferredContext(VulkanBase &device);

    ~DeferredContext() { Release(); }

    DeferredContext(const DeferredContext &) = delete;

    DeferredContext &operator=(const DeferredContext &) = delete;

    DeferredContext(DeferredContext &&other) noexcept { Swap(other); }

    DeferredContext &operator=(DeferredContext &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(DeferredContext &other) noexcept;

    [[nodiscard]] const vk::Extent2D &GetExtent() const { return m_extent; }

    [[nodiscard]] const vk::RenderPass &GetDeferredRenderPass() const { return m_deferredRenderPass; }

    [[nodiscard]] const vk::DescriptorSetLayout &GetDeferredTextureSetLayout() const { return m_deferredTextureSetLayout; }

    [[nodiscard]] const vk::DescriptorSet &GetDeferredTextureSet(uint32_t bufferingIndex) const {
        return m_deferredFramebuffers[bufferingIndex].GetTextureSet();
    }

    [[nodiscard]] const vk::RenderPassBeginInfo *GetDeferredRenderPassBeginInfo(uint32_t bufferingIndex) const {
        return &m_deferredRenderPassBeginInfo[bufferingIndex];
    }

    [[nodiscard]] const vk::RenderPass &GetForwardRenderPass() const { return m_forwardRenderPass; }

    [[nodiscard]] const vk::DescriptorSetLayout &GetForwardTextureSetLayout() const { return m_forwardTextureSetLayout; }

    [[nodiscard]] const vk::DescriptorSet &GetForwardTextureSet(uint32_t bufferingIndex) const {
        return m_forwardFramebuffers[bufferingIndex].GetTextureSet();
    }

    [[nodiscard]] const vk::RenderPassBeginInfo *GetForwardRenderPassBeginInfo(uint32_t bufferingIndex) const {
        return &m_forwardRenderPassBeginInfo[bufferingIndex];
    }

    void CheckFramebuffersOutOfDate();

private:
    void CreateRenderPass();

    void CreateFramebuffers();

    void CleanupFramebuffers();

    VulkanBase *m_device = nullptr;

    vk::Extent2D m_extent;
    vk::Sampler  m_sampler;

    vk::RenderPass          m_deferredRenderPass;
    vk::DescriptorSetLayout m_deferredTextureSetLayout;
    vk::RenderPass          m_forwardRenderPass;
    vk::DescriptorSetLayout m_forwardTextureSetLayout;

    std::vector<DeferredFramebuffer>     m_deferredFramebuffers;
    std::vector<vk::RenderPassBeginInfo> m_deferredRenderPassBeginInfo;
    std::vector<ForwardFramebuffer>      m_forwardFramebuffers;
    std::vector<vk::RenderPassBeginInfo> m_forwardRenderPassBeginInfo;
};
