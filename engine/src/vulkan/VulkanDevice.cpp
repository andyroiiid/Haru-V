//
// Created by andyroiiid on 12/18/2022.
//

#include "vulkan/VulkanDevice.h"

#include <GLFW/glfw3.h>

#include "core/DebugVk.h"

VulkanDevice::VulkanDevice(GLFWwindow *window) {
    m_window = window;
    CreateInstance();
    CreateDebugMessenger();
    PickPhysicalDevice();
    CreateDevice();
    CreateCommandPool();
    CreateAllocator();
    CreateImmediateContext();
    CreateSurface();
    CreateSwapchain();
    CreateSwapchainImageViews();
    CreatePrimaryRenderPass();
    CreatePrimaryFramebuffers();
    CreatePrimaryRenderPassBeginInfos();
    CreateBufferingObjects();
}

static std::vector<const char *> GetEnabledInstanceLayers() {
    return {
            "VK_LAYER_KHRONOS_validation"
    };
}

static std::vector<const char *> GetEnabledInstanceExtensions() {
    uint32_t numGlfwExtensions = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&numGlfwExtensions);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + numGlfwExtensions);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
}

void VulkanDevice::CreateInstance() {
    vk::ApplicationInfo applicationInfo(
            "Haru-V",
            VK_MAKE_VERSION(1, 0, 0),
            "Haru-V",
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_3
    );

    std::vector<const char *> enabledLayers = GetEnabledInstanceLayers();
    std::vector<const char *> enabledExtensions = GetEnabledInstanceExtensions();

    vk::InstanceCreateInfo instanceCreateInfo(
            {},
            &applicationInfo,
            enabledLayers.size(),
            enabledLayers.data(),
            enabledExtensions.size(),
            enabledExtensions.data()
    );

    m_instance = vk::createInstance(instanceCreateInfo);
    DebugCheckCritical(m_instance, "Failed to create Vulkan instance.");
}

[[maybe_unused]] static VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pMessenger
) {
    auto proc = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    DebugCheckCritical(proc != nullptr, "Failed to fetch vkCreateDebugUtilsMessengerEXT address.");
    return proc(instance, pCreateInfo, pAllocator, pMessenger);
}

[[maybe_unused]] static VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT messenger,
        const VkAllocationCallbacks *pAllocator
) {
    auto proc = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    DebugCheckCritical(proc != nullptr, "Failed to fetch vkDestroyDebugUtilsMessengerEXT address.");
    return proc(instance, messenger, pAllocator);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
        [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        [[maybe_unused]] void *pUserData
) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        DebugError("Vulkan: {}", pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        DebugWarning("Vulkan: {}", pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        DebugInfo("Vulkan: {}", pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        DebugVerbose("Vulkan: {}", pCallbackData->pMessage);
    } else {
        DebugError("Vulkan debug message with unknown severity level 0x{:X}: {}", messageSeverity, pCallbackData->pMessage);
    }
    return VK_FALSE;
}

void VulkanDevice::CreateDebugMessenger() {
    vk::DebugUtilsMessengerCreateInfoEXT createInfo(
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            VulkanDebugCallback
    );
    m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(createInfo);
    DebugCheckCritical(m_debugMessenger, "Failed to create Vulkan debug messenger.");
}

void VulkanDevice::PickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> physicalDevices = m_instance.enumeratePhysicalDevices();
    DebugCheck(!physicalDevices.empty(), "Can't find any Vulkan physical device.");

    // select the first discrete gpu or the first physical device
    vk::PhysicalDevice result = physicalDevices.front();
    for (vk::PhysicalDevice &physicalDevice: physicalDevices) {
        if (physicalDevice.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            result = physicalDevice;
            break;
        }
    }
    m_physicalDevice = result;

    std::vector<vk::QueueFamilyProperties> queueFamilies = m_physicalDevice.getQueueFamilyProperties();
    m_graphicsQueueFamily = 0;
    for (const vk::QueueFamilyProperties &queueFamily: queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            break;
        }
        m_graphicsQueueFamily++;
    }
    DebugCheckCritical(m_graphicsQueueFamily != queueFamilies.size(), "Failed to find a Vulkan graphics queue family.");
}

static std::vector<const char *> GetEnabledDeviceExtensions() {
    return {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
}

void VulkanDevice::CreateDevice() {
    float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo({}, m_graphicsQueueFamily, 1, &queuePriority);

    std::vector<const char *> enabledExtensions = GetEnabledDeviceExtensions();

    vk::PhysicalDeviceFeatures features;
    features.fillModeNonSolid = VK_TRUE;

    vk::DeviceCreateInfo createInfo(
            {},
            1,
            &queueCreateInfo,
            0,
            nullptr,
            enabledExtensions.size(),
            enabledExtensions.data(),
            &features
    );
    m_device = m_physicalDevice.createDevice(createInfo);
    DebugCheckCritical(m_device, "Failed to create Vulkan device.");

    m_graphicsQueue = m_device.getQueue(m_graphicsQueueFamily, 0);
    DebugCheckCritical(m_device, "Failed to get Vulkan graphics queue.");
}

void VulkanDevice::CreateCommandPool() {
    vk::CommandPoolCreateInfo createInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_graphicsQueueFamily);
    m_commandPool = m_device.createCommandPool(createInfo);
    DebugCheckCritical(m_commandPool, "Failed to create Vulkan command pool.");
}

vk::CommandBuffer VulkanDevice::AllocateCommandBuffer() {
    vk::CommandBufferAllocateInfo allocateInfo(m_commandPool, vk::CommandBufferLevel::ePrimary, 1);
    vk::CommandBuffer commandBuffer;
    DebugCheckCriticalVk(
            m_device.allocateCommandBuffers(&allocateInfo, &commandBuffer),
            "Failed to allocate Vulkan command buffer."
    );
    return commandBuffer;
}

void VulkanDevice::CreateAllocator() {
    VmaAllocatorCreateInfo createInfo{};
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.device = m_device;
    createInfo.instance = m_instance;
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    DebugCheckCriticalVk(
            vmaCreateAllocator(&createInfo, &m_allocator),
            "Failed to create Vulkan Memory Allocator."
    );
}

void VulkanDevice::CreateImmediateContext() {
    m_immediateFence = m_device.createFence({});
    DebugCheckCritical(m_immediateFence, "Failed to create Vulkan immediate fence.");
    m_immediateCommandBuffer = AllocateCommandBuffer();
}

void VulkanDevice::WaitAndResetFence(vk::Fence fence) {
    DebugCheckCriticalVk(
            m_device.waitForFences(fence, true, 100'000'000),
            "Failed to wait for Vulkan fence."
    );
    m_device.resetFences(fence);
}

void VulkanDevice::CreateSurface() {
    VkSurfaceKHR surface;
    DebugCheckCriticalVk(
            glfwCreateWindowSurface(m_instance, m_window, nullptr, &surface),
            "Failed to create Vulkan surface."
    );
    m_surface = surface;
}

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

void VulkanDevice::CreateSwapchain() {
    vk::SurfaceCapabilitiesKHR capabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
    m_swapchainExtent = CalcSwapchainExtent(capabilities, m_window);

    vk::SwapchainCreateInfoKHR createInfo(
            {},
            m_surface,
            imageCount,
            SURFACE_FORMAT,
            SURFACE_COLOR_SPACE,
            m_swapchainExtent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            0,
            nullptr,
            capabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            PRESENT_MODE,
            VK_TRUE,
            m_swapchain
    );
    m_swapchain = m_device.createSwapchainKHR(createInfo);
    DebugCheckCritical(m_swapchain, "Failed to create Vulkan swapchain.");

    m_swapchainImages = m_device.getSwapchainImagesKHR(m_swapchain);
}

void VulkanDevice::CreateSwapchainImageViews() {
    vk::ImageViewCreateInfo createInfo(
            {},
            {},
            vk::ImageViewType::e2D,
            SURFACE_FORMAT,
            {},
            {
                    vk::ImageAspectFlagBits::eColor,
                    0,
                    1,
                    0,
                    1
            }
    );
    for (vk::Image image: m_swapchainImages) {
        createInfo.image = image;
        m_swapchainImageViews.push_back(m_device.createImageView(createInfo));
    }
}

void VulkanDevice::CreatePrimaryRenderPass() {
    vk::AttachmentDescription colorAttachment(
            {},
            SURFACE_FORMAT,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::ePresentSrcKHR
    );
    vk::AttachmentReference colorAttachmentRef(
            0,
            vk::ImageLayout::eColorAttachmentOptimal
    );
    vk::SubpassDescription subpass(
            {},
            vk::PipelineBindPoint::eGraphics,
            {},
            colorAttachmentRef
    );
    vk::RenderPassCreateInfo createInfo(
            {},
            colorAttachment,
            subpass
    );
    m_primaryRenderPass = m_device.createRenderPass(createInfo);
    DebugCheckCritical(m_primaryRenderPass, "Failed to create primary Vulkan render pass.");
}

void VulkanDevice::CreatePrimaryFramebuffers() {
    vk::FramebufferCreateInfo createInfo(
            {},
            m_primaryRenderPass,
            {},
            m_swapchainExtent.width,
            m_swapchainExtent.height,
            1
    );
    for (vk::ImageView imageView: m_swapchainImageViews) {
        createInfo.setAttachments(imageView);
        m_primaryFramebuffers.push_back(m_device.createFramebuffer(createInfo));
    }
}

void VulkanDevice::CreatePrimaryRenderPassBeginInfos() {
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

void VulkanDevice::CreateBufferingObjects() {
    for (BufferingObjects &bufferingObject: m_bufferingObjects) {
        bufferingObject.RenderFence = m_device.createFence({vk::FenceCreateFlagBits::eSignaled});
        bufferingObject.PresentSemaphore = m_device.createSemaphore({});
        bufferingObject.RenderSemaphore = m_device.createSemaphore({});
        bufferingObject.CommandBuffer = AllocateCommandBuffer();
    }
}

VulkanDevice::~VulkanDevice() {
    m_device.waitIdle();

    for (BufferingObjects &bufferingObject: m_bufferingObjects) {
        m_device.destroy(bufferingObject.RenderFence);
        m_device.destroy(bufferingObject.PresentSemaphore);
        m_device.destroy(bufferingObject.RenderSemaphore);
        m_device.free(m_commandPool, bufferingObject.CommandBuffer);
    }
    for (vk::Framebuffer framebuffer: m_primaryFramebuffers) {
        m_device.destroy(framebuffer);
    }
    m_device.destroy(m_primaryRenderPass);
    for (vk::ImageView imageView: m_swapchainImageViews) {
        m_device.destroy(imageView);
    }
    m_device.destroy(m_swapchain);
    m_instance.destroy(m_surface);
    m_device.free(m_commandPool, m_immediateCommandBuffer);
    m_device.destroy(m_immediateFence);
    vmaDestroyAllocator(m_allocator);
    m_device.destroy(m_commandPool);
    m_device.destroy();
    m_instance.destroy(m_debugMessenger);
    m_instance.destroy();
}

VulkanDevice::BeginFrameInfo VulkanDevice::BeginFrame() {
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

void VulkanDevice::EndFrame() {
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
