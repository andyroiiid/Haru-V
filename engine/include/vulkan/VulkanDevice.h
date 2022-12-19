//
// Created by andyroiiid on 12/18/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

struct GLFWwindow;

class VulkanDevice {
public:
    static constexpr vk::Format SURFACE_FORMAT = vk::Format::eB8G8R8A8Unorm;
    static constexpr vk::ColorSpaceKHR SURFACE_COLOR_SPACE = vk::ColorSpaceKHR::eSrgbNonlinear;
    static constexpr vk::PresentModeKHR PRESENT_MODE = vk::PresentModeKHR::eMailbox;

    explicit VulkanDevice(GLFWwindow *window);

    ~VulkanDevice();

    VulkanDevice(const VulkanDevice &) = delete;

    VulkanDevice &operator=(const VulkanDevice &) = delete;

    VulkanDevice(VulkanDevice &&) = delete;

    VulkanDevice &operator=(VulkanDevice &&) = delete;

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
        m_immediateCommandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
        func(m_immediateCommandBuffer);
        m_immediateCommandBuffer.end();

        vk::SubmitInfo submitInfo({}, {}, m_immediateCommandBuffer, {});
        m_graphicsQueue.submit(submitInfo, m_immediateFence);
        WaitAndResetFence(m_immediateFence);
    }

private:
    void CreateInstance();

    void CreateDebugMessenger();

    void PickPhysicalDevice();

    void CreateDevice();

    void CreateCommandPool();

    vk::CommandBuffer AllocateCommandBuffer();

    void CreateAllocator();

    void CreateImmediateContext();

    void WaitAndResetFence(vk::Fence fence);

    void CreateSurface();

    void CreateSwapchain();

    void CreateSwapchainImageViews();

    void CreatePrimaryRenderPass();

    void CreatePrimaryFramebuffers();

    void CreatePrimaryRenderPassBeginInfos();

    void CreateBufferingObjects();

    GLFWwindow *m_window = nullptr;

    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;

    vk::PhysicalDevice m_physicalDevice;
    uint32_t m_graphicsQueueFamily = 0;
    vk::Device m_device;
    vk::Queue m_graphicsQueue;

    vk::CommandPool m_commandPool;

    VmaAllocator m_allocator = VK_NULL_HANDLE;

    vk::Fence m_immediateFence;
    vk::CommandBuffer m_immediateCommandBuffer;

    vk::SurfaceKHR m_surface;

    vk::SwapchainKHR m_swapchain;
    vk::Extent2D m_swapchainExtent;
    std::vector<vk::Image> m_swapchainImages;
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
