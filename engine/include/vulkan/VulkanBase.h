//
// Created by andyroiiid on 12/20/2022.
//

#pragma once

#include "vulkan/VulkanDevice.h"

class VulkanBase : public VulkanDevice {
public:
    static constexpr vk::Format SURFACE_FORMAT = vk::Format::eB8G8R8A8Unorm;
    static constexpr vk::ColorSpaceKHR SURFACE_COLOR_SPACE = vk::ColorSpaceKHR::eSrgbNonlinear;
    static constexpr vk::PresentModeKHR PRESENT_MODE = vk::PresentModeKHR::eMailbox;

    explicit VulkanBase(GLFWwindow *window);

    ~VulkanBase();

    VulkanBase(const VulkanBase &) = delete;

    VulkanBase &operator=(const VulkanBase &) = delete;

    VulkanBase(VulkanBase &&) = delete;

    VulkanBase &operator=(VulkanBase &&) = delete;

    [[nodiscard]] const vk::RenderPass &GetPrimaryRenderPass() const { return m_primaryRenderPass; }

    [[nodiscard]] const vk::Extent2D &GetSwapchainExtent() const { return m_swapchainExtent; }

    [[nodiscard]] size_t GetNumBuffering() const { return m_bufferingObjects.size(); }

    struct BeginFrameInfo {
        [[maybe_unused]] const vk::RenderPassBeginInfo *PrimaryRenderPassBeginInfo = nullptr;
        [[maybe_unused]] uint32_t BufferingIndex = 0;
        [[maybe_unused]] vk::CommandBuffer CommandBuffer;
    };

    BeginFrameInfo BeginFrame();

    void EndFrame();

    template<class Func>
    void ImmediateSubmit(Func &&func) {
        m_immediateCommandBuffer.reset();
        BeginCommandBuffer(m_immediateCommandBuffer, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        func(m_immediateCommandBuffer);
        EndCommandBuffer(m_immediateCommandBuffer);

        const vk::SubmitInfo submitInfo({}, {}, m_immediateCommandBuffer, {});
        SubmitToGraphicsQueue(submitInfo, m_immediateFence);
        WaitAndResetFence(m_immediateFence);
    }

private:
    void CreateImmediateContext();

    void CreateSurfaceSwapchainAndImageViews();

    void CreatePrimaryRenderPassAndFramebuffers();

    void CreateBufferingObjects();

    GLFWwindow *m_window = nullptr;

    vk::Fence m_immediateFence;
    vk::CommandBuffer m_immediateCommandBuffer;

    vk::SurfaceKHR m_surface;

    vk::Extent2D m_swapchainExtent;
    vk::SwapchainKHR m_swapchain;
    std::vector<vk::ImageView> m_swapchainImageViews;

    vk::RenderPass m_primaryRenderPass;
    std::vector<vk::Framebuffer> m_primaryFramebuffers;
    std::vector<vk::RenderPassBeginInfo> m_primaryRenderPassBeginInfos;

    struct BufferingObjects {
        vk::Fence RenderFence;
        vk::Semaphore PresentSemaphore;
        vk::Semaphore RenderSemaphore;
        vk::CommandBuffer CommandBuffer;
    };
    std::array<BufferingObjects, 3> m_bufferingObjects;

    uint32_t m_currentSwapchainImageIndex = 0;
    uint32_t m_currentBufferingIndex = 0;
};
