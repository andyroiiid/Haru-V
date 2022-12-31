//
// Created by andyroiiid on 12/20/2022.
//

#pragma once

#include "vulkan/VulkanDevice.h"
#include "vulkan/VulkanImage.h"

static inline std::pair<vk::Viewport, vk::Rect2D> CalcViewportAndScissorFromExtent(const vk::Extent2D &extent, bool flipped = true) {
    // flipped upside down so that it's consistent with OpenGL
    const auto width = static_cast<float>(extent.width);
    const auto height = static_cast<float>(extent.height);
    return {
            {
                    0.0f,
                    flipped ? height : 0,
                    width,
                    flipped ? -height : height,
                    0.0f,
                    1.0f
            },
            {
                    {0, 0},
                    extent
            }
    };
}

struct VulkanFrameInfo {
    const vk::RenderPassBeginInfo *PrimaryRenderPassBeginInfo = nullptr;
    uint32_t BufferingIndex = 0;
    vk::CommandBuffer CommandBuffer;
};

class VulkanBase : public VulkanDevice {
public:
    static constexpr vk::Format SURFACE_FORMAT = vk::Format::eB8G8R8A8Unorm;
    static constexpr vk::ColorSpaceKHR SURFACE_COLOR_SPACE = vk::ColorSpaceKHR::eSrgbNonlinear;
    static constexpr vk::Format DEPTH_FORMAT = vk::Format::eD32Sfloat;
    static constexpr vk::PresentModeKHR PRESENT_MODE = vk::PresentModeKHR::eFifo;

    explicit VulkanBase(GLFWwindow *window);

    ~VulkanBase();

    VulkanBase(const VulkanBase &) = delete;

    VulkanBase &operator=(const VulkanBase &) = delete;

    VulkanBase(VulkanBase &&) = delete;

    VulkanBase &operator=(VulkanBase &&) = delete;

    [[nodiscard]] const vk::RenderPass &GetPrimaryRenderPass() const { return m_primaryRenderPass; }

    [[nodiscard]] const vk::Extent2D &GetSwapchainExtent() const { return m_swapchainExtent; }

    [[nodiscard]] size_t GetNumBuffering() const { return m_bufferingObjects.size(); }

    VulkanFrameInfo BeginFrame();

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

    void CreateBufferingObjects();

    void CreateSurfaceSwapchainAndImageViews();

    void CreatePrimaryRenderPass();

    void CreatePrimaryFramebuffers();

    void CleanupSurfaceSwapchainAndImageViews();

    void CleanupPrimaryFramebuffers();

    void RecreateSwapchain();

    vk::Result TryAcquiringNextSwapchainImage();

    void AcquireNextSwapchainImage();

    GLFWwindow *m_window = nullptr;

    vk::Fence m_immediateFence;
    vk::CommandBuffer m_immediateCommandBuffer;

    struct BufferingObjects {
        vk::Fence RenderFence;
        vk::Semaphore PresentSemaphore;
        vk::Semaphore RenderSemaphore;
        vk::CommandBuffer CommandBuffer;
    };
    std::array<BufferingObjects, 3> m_bufferingObjects;

    vk::SurfaceKHR m_surface;
    vk::Extent2D m_swapchainExtent;
    vk::SwapchainKHR m_swapchain;
    std::vector<vk::ImageView> m_swapchainImageViews;
    std::vector<VulkanImage> m_depthImages;
    std::vector<vk::ImageView> m_depthImageViews;

    vk::RenderPass m_primaryRenderPass;

    std::vector<vk::Framebuffer> m_primaryFramebuffers;
    std::vector<vk::RenderPassBeginInfo> m_primaryRenderPassBeginInfos;

    uint32_t m_currentSwapchainImageIndex = 0;
    uint32_t m_currentBufferingIndex = 0;
};
