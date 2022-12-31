//
// Created by andyroiiid on 12/20/2022.
//

// !!! WARNING: You'll probably want to read this file with parameter hints
// !!! https://www.jetbrains.com/help/clion/parameter-hints.html

#include "vulkan/VulkanBase.h"

#include <GLFW/glfw3.h>

#include "vulkan/DebugVk.h"

static vk::Extent2D CalcSwapchainExtent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {
    if (capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max() &&
        capabilities.currentExtent.height == std::numeric_limits<uint32_t>::max()) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const vk::Extent2D extent(
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
    CreateBufferingObjects();
    CreateSurfaceSwapchainAndImageViews();
    CreatePrimaryRenderPass();
    CreatePrimaryFramebuffers();
}

void VulkanBase::CreateImmediateContext() {
    m_immediateFence = CreateFence();
    m_immediateCommandBuffer = AllocateCommandBuffer();
}

void VulkanBase::CreateBufferingObjects() {
    for (BufferingObjects &bufferingObject: m_bufferingObjects) {
        bufferingObject.RenderFence = CreateFence(vk::FenceCreateFlagBits::eSignaled);
        bufferingObject.PresentSemaphore = CreateSemaphore();
        bufferingObject.RenderSemaphore = CreateSemaphore();
        bufferingObject.CommandBuffer = AllocateCommandBuffer();
    }
}

void VulkanBase::CreateSurfaceSwapchainAndImageViews() {
    m_surface = CreateSurface(m_window);

    const auto [capabilitiesResult, capabilities] = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
    DebugCheckCriticalVk(capabilitiesResult, "Failed to get Vulkan surface capabilities.");
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

    const auto [swapchainImagesResult, swapchainImages] = m_device.getSwapchainImagesKHR(m_swapchain);
    DebugCheckCriticalVk(swapchainImagesResult, "Failed to get Vulkan swapchain images.");
    for (const vk::Image &image: swapchainImages) {
        m_swapchainImageViews.push_back(CreateImageView(
                image,
                SURFACE_FORMAT,
                vk::ImageAspectFlagBits::eColor
        ));
        VulkanImage depthImage = CreateImage(
                DEPTH_FORMAT,
                m_swapchainExtent,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                0,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        );
        m_depthImageViews.push_back(CreateImageView(
                depthImage.Get(),
                DEPTH_FORMAT,
                vk::ImageAspectFlagBits::eDepth
        ));
        m_depthImages.push_back(std::move(depthImage));
    }
}

void VulkanBase::CreatePrimaryRenderPass() {
    m_primaryRenderPass = CreateRenderPass(
            {
                    SURFACE_FORMAT
            },
            DEPTH_FORMAT,
            false,
            true
    );
}

void VulkanBase::CreatePrimaryFramebuffers() {
    const size_t numSwapchainImages = m_swapchainImageViews.size();
    for (int i = 0; i < numSwapchainImages; i++) {
        vk::ImageView attachmentViews[]{
                m_swapchainImageViews[i],
                m_depthImageViews[i]
        };
        m_primaryFramebuffers.push_back(CreateFramebuffer(
                m_primaryRenderPass,
                attachmentViews,
                m_swapchainExtent
        ));
    }

    static vk::ClearValue clearValues[]{
            {vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}},
            {vk::ClearDepthStencilValue{1.0f, 0}}
    };
    vk::RenderPassBeginInfo beginInfo(
            m_primaryRenderPass,
            {},
            {{0, 0}, m_swapchainExtent},
            clearValues
    );
    for (const vk::Framebuffer &framebuffer: m_primaryFramebuffers) {
        beginInfo.framebuffer = framebuffer;
        m_primaryRenderPassBeginInfos.push_back(beginInfo);
    }
}

void VulkanBase::CleanupSurfaceSwapchainAndImageViews() {
    for (const vk::ImageView &imageView: m_depthImageViews) {
        m_device.destroy(imageView);
    }
    m_depthImageViews.clear();
    m_depthImages.clear();
    for (const vk::ImageView &imageView: m_swapchainImageViews) {
        m_device.destroy(imageView);
    }
    m_swapchainImageViews.clear();
    m_device.destroy(m_swapchain);
    m_swapchain = VK_NULL_HANDLE;
    m_instance.destroy(m_surface);
    m_surface = VK_NULL_HANDLE;
}

void VulkanBase::CleanupPrimaryFramebuffers() {
    m_primaryRenderPassBeginInfos.clear();
    for (const vk::Framebuffer &framebuffer: m_primaryFramebuffers) {
        DestroyFramebuffer(framebuffer);
    }
    m_primaryFramebuffers.clear();
}

void VulkanBase::RecreateSwapchain() {
    // block when minimized
    while (true) {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        if (width != 0 && height != 0) {
            break;
        }
        glfwWaitEvents();
    }

    WaitIdle();

    CleanupPrimaryFramebuffers();
    CleanupSurfaceSwapchainAndImageViews();
    CreateSurfaceSwapchainAndImageViews();
    CreatePrimaryFramebuffers();
}

VulkanBase::~VulkanBase() {
    WaitIdle();

    for (const BufferingObjects &bufferingObject: m_bufferingObjects) {
        m_device.destroy(bufferingObject.RenderFence);
        m_device.destroy(bufferingObject.PresentSemaphore);
        m_device.destroy(bufferingObject.RenderSemaphore);
        m_device.free(m_commandPool, bufferingObject.CommandBuffer);
    }

    CleanupPrimaryFramebuffers();
    DestroyRenderPass(m_primaryRenderPass);
    CleanupSurfaceSwapchainAndImageViews();

    m_device.free(m_commandPool, m_immediateCommandBuffer);
    m_device.destroy(m_immediateFence);
}

vk::Result VulkanBase::TryAcquiringNextSwapchainImage() {
    const BufferingObjects &bufferingObjects = m_bufferingObjects[m_currentBufferingIndex];

    return m_device.acquireNextImageKHR(
            m_swapchain,
            100'000'000,
            bufferingObjects.PresentSemaphore,
            nullptr,
            &m_currentSwapchainImageIndex
    );
}

void VulkanBase::AcquireNextSwapchainImage() {
    vk::Result result = TryAcquiringNextSwapchainImage();
    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        if (result == vk::Result::eErrorOutOfDateKHR) {
            // recreate swapchain and try acquiring again
            RecreateSwapchain();
            result = TryAcquiringNextSwapchainImage();
        }

        DebugCheckCriticalVk(
                result,
                "Failed to acquire next Vulkan swapchain image."
        );
    }
}

VulkanFrameInfo VulkanBase::BeginFrame() {
    const BufferingObjects &bufferingObjects = m_bufferingObjects[m_currentBufferingIndex];

    WaitAndResetFence(bufferingObjects.RenderFence);

    AcquireNextSwapchainImage();

    bufferingObjects.CommandBuffer.reset();
    BeginCommandBuffer(bufferingObjects.CommandBuffer, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    return {
            &m_primaryRenderPassBeginInfos[m_currentSwapchainImageIndex],
            m_currentBufferingIndex,
            bufferingObjects.CommandBuffer
    };
}

void VulkanBase::EndFrame() {
    BufferingObjects &bufferingObjects = m_bufferingObjects[m_currentBufferingIndex];

    EndCommandBuffer(bufferingObjects.CommandBuffer);

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    const vk::SubmitInfo submitInfo(
            bufferingObjects.PresentSemaphore,
            waitStage,
            bufferingObjects.CommandBuffer,
            bufferingObjects.RenderSemaphore
    );
    SubmitToGraphicsQueue(submitInfo, bufferingObjects.RenderFence);

    const vk::PresentInfoKHR presentInfo(
            bufferingObjects.RenderSemaphore,
            m_swapchain,
            m_currentSwapchainImageIndex
    );
    const vk::Result result = m_graphicsQueue.presentKHR(presentInfo);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        RecreateSwapchain();
    } else {
        DebugCheckCriticalVk(
                result,
                "Failed to present Vulkan swapchain image."
        );
    }

    m_currentBufferingIndex = (m_currentBufferingIndex + 1) % m_bufferingObjects.size();
}
