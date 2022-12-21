//
// Created by andyroiiid on 12/20/2022.
//

#include "vulkan/VulkanBase.h"

#include <GLFW/glfw3.h>

#include "core/DebugVk.h"

static vk::Extent2D CalcSwapchainExtent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {
    if (capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max() &&
        capabilities.currentExtent.height == std::numeric_limits<uint32_t>::max()) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        vk::Extent2D extent(
                std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        );
        return extent;
    }
    return capabilities.currentExtent;
}

VulkanBase::VulkanBase(GLFWwindow *window)
        : m_window(window) {
    CreateImmediateContext();
    CreateSurfaceSwapchainAndImageViews();
    CreatePrimaryRenderPassAndFramebuffers();
    CreateBufferingObjects();
}

void VulkanBase::CreateImmediateContext() {
    m_immediateFence = CreateFence();
    m_immediateCommandBuffer = AllocateCommandBuffer();
}

void VulkanBase::CreateSurfaceSwapchainAndImageViews() {
    m_surface = CreateSurface(m_window);

    vk::SurfaceCapabilitiesKHR capabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
    m_swapchainExtent = CalcSwapchainExtent(capabilities, m_window);

    m_swapchain = CreateSwapchain(
            m_surface,
            imageCount,
            SURFACE_FORMAT,
            SURFACE_COLOR_SPACE,
            m_swapchainExtent,
            capabilities.currentTransform,
            PRESENT_MODE,
            m_swapchain
    );

    for (vk::Image image: m_device.getSwapchainImagesKHR(m_swapchain)) {
        m_swapchainImageViews.push_back(CreateImageView(image, SURFACE_FORMAT, vk::ImageAspectFlagBits::eColor));
    }
}

void VulkanBase::CreatePrimaryRenderPassAndFramebuffers() {
    m_primaryRenderPass = CreateRenderPass(
            {
                    SURFACE_FORMAT
            },
            vk::Format::eUndefined,
            true
    );

    for (vk::ImageView imageView: m_swapchainImageViews) {
        m_primaryFramebuffers.push_back(CreateFramebuffer(m_primaryRenderPass, {imageView}, m_swapchainExtent));
    }

    static vk::ClearValue clearValues[]{
            {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}
    };
    vk::RenderPassBeginInfo beginInfo(
            m_primaryRenderPass,
            {},
            {{0, 0}, m_swapchainExtent},
            clearValues
    );
    for (vk::Framebuffer framebuffer: m_primaryFramebuffers) {
        beginInfo.framebuffer = framebuffer;
        m_primaryRenderPassBeginInfos.push_back(beginInfo);
    }
}

void VulkanBase::CreateBufferingObjects() {
    for (BufferingObjects &bufferingObject: m_bufferingObjects) {
        bufferingObject.RenderFence = CreateFence(vk::FenceCreateFlagBits::eSignaled);
        bufferingObject.PresentSemaphore = CreateSemaphore();
        bufferingObject.RenderSemaphore = CreateSemaphore();
        bufferingObject.CommandBuffer = AllocateCommandBuffer();
    }
}

VulkanBase::~VulkanBase() {
    WaitIdle();

    for (BufferingObjects &bufferingObject: m_bufferingObjects) {
        m_device.destroy(bufferingObject.RenderFence);
        m_device.destroy(bufferingObject.PresentSemaphore);
        m_device.destroy(bufferingObject.RenderSemaphore);
        m_device.free(m_commandPool, bufferingObject.CommandBuffer);
    }

    for (vk::Framebuffer framebuffer: m_primaryFramebuffers) {
        DestroyFramebuffer(framebuffer);
    }
    DestroyRenderPass(m_primaryRenderPass);

    for (vk::ImageView imageView: m_swapchainImageViews) {
        m_device.destroy(imageView);
    }
    m_device.destroy(m_swapchain);
    m_instance.destroy(m_surface);

    m_device.free(m_commandPool, m_immediateCommandBuffer);
    m_device.destroy(m_immediateFence);
}

VulkanBase::BeginFrameInfo VulkanBase::BeginFrame() {
    BufferingObjects &bufferingObjects = m_bufferingObjects[m_currentBufferingIndex];

    WaitAndResetFence(bufferingObjects.RenderFence);

    DebugCheckCriticalVk(
            m_device.acquireNextImageKHR(m_swapchain, 100'000'000, bufferingObjects.PresentSemaphore, nullptr, &m_currentSwapchainImageIndex),
            "Failed to acquire next Vulkan swapchain image."
    );

    bufferingObjects.CommandBuffer.reset();
    bufferingObjects.CommandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    return {
            &m_primaryRenderPassBeginInfos[m_currentSwapchainImageIndex],
            m_currentBufferingIndex,
            bufferingObjects.CommandBuffer
    };
}

void VulkanBase::EndFrame() {
    BufferingObjects &bufferingObjects = m_bufferingObjects[m_currentBufferingIndex];

    bufferingObjects.CommandBuffer.end();

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo(
            bufferingObjects.PresentSemaphore,
            waitStage,
            bufferingObjects.CommandBuffer,
            bufferingObjects.RenderSemaphore
    );
    m_graphicsQueue.submit(submitInfo, bufferingObjects.RenderFence);

    vk::PresentInfoKHR presentInfo(
            bufferingObjects.RenderSemaphore,
            m_swapchain,
            m_currentSwapchainImageIndex
    );
    DebugCheckCriticalVk(
            m_graphicsQueue.presentKHR(presentInfo),
            "Failed to present Vulkan swapchain image."
    );

    m_currentBufferingIndex = (m_currentBufferingIndex + 1) % m_bufferingObjects.size();
}
