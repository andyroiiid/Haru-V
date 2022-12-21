//
// Created by andyroiiid on 12/18/2022.
//

// !!! WARNING: You'll probably want to read this file with parameter hints
// !!! https://www.jetbrains.com/help/clion/parameter-hints.html

#include "vulkan/VulkanDevice.h"

#include <GLFW/glfw3.h>

#include "core/DebugVk.h"

VulkanDevice::VulkanDevice() {
    CreateInstance();
    CreateDebugMessenger();
    PickPhysicalDevice();
    CreateDevice();
    CreateCommandPool();
    CreateAllocator();
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
    const vk::ApplicationInfo applicationInfo(
            "Haru-V",
            VK_MAKE_VERSION(1, 0, 0),
            "Haru-V",
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_3
    );

    const std::vector<const char *> enabledLayers = GetEnabledInstanceLayers();
    const std::vector<const char *> enabledExtensions = GetEnabledInstanceExtensions();

    const vk::InstanceCreateInfo instanceCreateInfo(
            {},
            &applicationInfo,
            enabledLayers.size(),
            enabledLayers.data(),
            enabledExtensions.size(),
            enabledExtensions.data()
    );

    const auto [result, instance] = vk::createInstance(instanceCreateInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan instance.");
    m_instance = instance;
}

[[maybe_unused]] static VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pMessenger
) {
    const auto proc = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    DebugCheckCritical(proc != nullptr, "Failed to fetch vkCreateDebugUtilsMessengerEXT address.");
    return proc(instance, pCreateInfo, pAllocator, pMessenger);
}

[[maybe_unused]] static VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT messenger,
        const VkAllocationCallbacks *pAllocator
) {
    const auto proc = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
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
    const vk::DebugUtilsMessengerCreateInfoEXT createInfo(
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            VulkanDebugCallback
    );
    const auto [result, debugMessenger] = m_instance.createDebugUtilsMessengerEXT(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan debug messenger.");
    m_debugMessenger = debugMessenger;
}

void VulkanDevice::PickPhysicalDevice() {
    const auto [result, physicalDevices] = m_instance.enumeratePhysicalDevices();
    DebugCheckCriticalVk(result, "Failed to enumerate Vulkan physical devices.");
    DebugCheck(!physicalDevices.empty(), "Can't find any Vulkan physical device.");

    // select the first discrete gpu or the first physical device
    vk::PhysicalDevice selected = physicalDevices.front();
    for (const vk::PhysicalDevice &physicalDevice: physicalDevices) {
        if (physicalDevice.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            selected = physicalDevice;
            break;
        }
    }
    m_physicalDevice = selected;

    const std::vector<vk::QueueFamilyProperties> queueFamilies = m_physicalDevice.getQueueFamilyProperties();
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
    const float queuePriority = 1.0f;
    const vk::DeviceQueueCreateInfo queueCreateInfo({}, m_graphicsQueueFamily, 1, &queuePriority);

    const std::vector<const char *> enabledExtensions = GetEnabledDeviceExtensions();

    vk::PhysicalDeviceFeatures features;
    features.fillModeNonSolid = VK_TRUE;

    const vk::DeviceCreateInfo createInfo(
            {},
            1,
            &queueCreateInfo,
            0,
            nullptr,
            enabledExtensions.size(),
            enabledExtensions.data(),
            &features
    );
    const auto [result, device] = m_physicalDevice.createDevice(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan device.");
    m_device = device;

    m_graphicsQueue = m_device.getQueue(m_graphicsQueueFamily, 0);
    DebugCheckCritical(m_device, "Failed to get Vulkan graphics queue.");
}

void VulkanDevice::CreateCommandPool() {
    const vk::CommandPoolCreateInfo createInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_graphicsQueueFamily);
    const auto [result, commandPool] = m_device.createCommandPool(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan command pool.");
    m_commandPool = commandPool;
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

VulkanDevice::~VulkanDevice() {
    WaitIdle();

    vmaDestroyAllocator(m_allocator);
    m_device.destroy(m_commandPool);
    m_device.destroy();
    m_instance.destroy(m_debugMessenger);
    m_instance.destroy();
}

void VulkanDevice::WaitIdle() {
    DebugCheckCriticalVk(
            m_device.waitIdle(),
            "Failed when waiting for Vulkan device to be idle."
    );
}

vk::Fence VulkanDevice::CreateFence(vk::FenceCreateFlags flags) {
    const vk::FenceCreateInfo createInfo(flags);
    const auto [result, fence] = m_device.createFence(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan fence.");
    return fence;
}

void VulkanDevice::WaitAndResetFence(vk::Fence fence, uint64_t timeout) {
    DebugCheckCriticalVk(
            m_device.waitForFences(fence, true, timeout),
            "Failed to wait for Vulkan fence."
    );
    m_device.resetFences(fence);
}

vk::Semaphore VulkanDevice::CreateSemaphore() {
    const vk::SemaphoreCreateInfo createInfo;
    const auto [result, semaphore] = m_device.createSemaphore(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan semaphore.");
    return semaphore;
}

vk::CommandBuffer VulkanDevice::AllocateCommandBuffer() {
    const vk::CommandBufferAllocateInfo allocateInfo(m_commandPool, vk::CommandBufferLevel::ePrimary, 1);
    vk::CommandBuffer commandBuffer;
    DebugCheckCriticalVk(
            m_device.allocateCommandBuffers(&allocateInfo, &commandBuffer),
            "Failed to allocate Vulkan command buffer."
    );
    return commandBuffer;
}

vk::SurfaceKHR VulkanDevice::CreateSurface(GLFWwindow *window) {
    VkSurfaceKHR surface;
    DebugCheckCriticalVk(
            glfwCreateWindowSurface(m_instance, window, nullptr, &surface),
            "Failed to create Vulkan surface."
    );
    return surface;
}

vk::SwapchainKHR VulkanDevice::CreateSwapchain(
        vk::SurfaceKHR surface,
        uint32_t imageCount,
        vk::Format imageFormat,
        vk::ColorSpaceKHR imageColorSpace,
        const vk::Extent2D &imageExtent,
        vk::SurfaceTransformFlagBitsKHR preTransform,
        vk::PresentModeKHR presentMode,
        vk::SwapchainKHR oldSwapchain
) {
    const vk::SwapchainCreateInfoKHR createInfo(
            {},
            surface,
            imageCount,
            imageFormat,
            imageColorSpace,
            imageExtent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            {},
            preTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            presentMode,
            VK_TRUE,
            oldSwapchain,
            nullptr
    );
    const auto [result, swapchain] = m_device.createSwapchainKHR(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan swapchain.");
    return swapchain;
}

vk::ImageView VulkanDevice::CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectMask) {
    const vk::ImageViewCreateInfo createInfo(
            {},
            image,
            vk::ImageViewType::e2D,
            format,
            {},
            {
                    aspectMask,
                    0,
                    1,
                    0,
                    1
            }
    );
    const auto [result, imageView] = m_device.createImageView(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan image view.");
    return imageView;
}

vk::RenderPass VulkanDevice::CreateRenderPass(
        const std::initializer_list<vk::Format> &colorAttachmentFormats,
        vk::Format depthStencilAttachmentFormat,
        bool forPresent
) {
    std::vector<vk::AttachmentDescription> attachments;
    std::vector<vk::AttachmentReference> colorAttachmentRefs;
    vk::AttachmentReference depthStencilAttachmentRef;

    uint32_t attachmentIndex = 0;
    for (const vk::Format &colorAttachmentFormat: colorAttachmentFormats) {
        attachments.emplace_back(
                vk::AttachmentDescriptionFlags{},
                colorAttachmentFormat,
                vk::SampleCountFlagBits::e1,
                vk::AttachmentLoadOp::eClear,
                vk::AttachmentStoreOp::eStore,
                vk::AttachmentLoadOp::eDontCare,
                vk::AttachmentStoreOp::eDontCare,
                vk::ImageLayout::eUndefined,
                forPresent ? vk::ImageLayout::ePresentSrcKHR : vk::ImageLayout::eShaderReadOnlyOptimal
        );

        colorAttachmentRefs.emplace_back(
                attachmentIndex++,
                vk::ImageLayout::eColorAttachmentOptimal
        );
    }

    if (depthStencilAttachmentFormat != vk::Format::eUndefined) {
        attachments.emplace_back(
                vk::AttachmentDescriptionFlags{},
                depthStencilAttachmentFormat,
                vk::SampleCountFlagBits::e1,
                vk::AttachmentLoadOp::eClear,
                vk::AttachmentStoreOp::eStore,
                vk::AttachmentLoadOp::eDontCare,
                vk::AttachmentStoreOp::eDontCare,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eDepthStencilAttachmentOptimal
        );

        depthStencilAttachmentRef = {
                attachmentIndex,
                vk::ImageLayout::eDepthStencilAttachmentOptimal
        };
    }

    vk::SubpassDescription subpass(
            {},
            vk::PipelineBindPoint::eGraphics,
            {},
            colorAttachmentRefs
    );

    if (depthStencilAttachmentFormat != vk::Format::eUndefined) {
        subpass.setPDepthStencilAttachment(&depthStencilAttachmentRef);
    }

    const vk::RenderPassCreateInfo createInfo(
            {},
            attachments,
            subpass
    );
    const auto [result, renderPass] = m_device.createRenderPass(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan render pass.");
    return renderPass;
}

void VulkanDevice::DestroyRenderPass(vk::RenderPass renderPass) {
    m_device.destroyRenderPass(renderPass);
}

vk::Framebuffer VulkanDevice::CreateFramebuffer(
        vk::RenderPass renderPass,
        const std::initializer_list<vk::ImageView> &attachments,
        const vk::Extent2D &extent
) {
    const vk::FramebufferCreateInfo createInfo(
            {},
            renderPass,
            attachments,
            extent.width,
            extent.height,
            1
    );
    const auto [result, framebuffer] = m_device.createFramebuffer(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan framebuffer.");
    return framebuffer;
}

void VulkanDevice::DestroyFramebuffer(vk::Framebuffer framebuffer) {
    m_device.destroyFramebuffer(framebuffer);
}

vk::ShaderModule VulkanDevice::CreateShaderModule(const std::vector<uint32_t> &spirv) {
    const vk::ShaderModuleCreateInfo createInfo(
            {},
            spirv.size() * sizeof(uint32_t),
            spirv.data()
    );
    const auto [result, shaderModule] = m_device.createShaderModule(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan shader module.");
    return shaderModule;
}

void VulkanDevice::DestroyShaderModule(vk::ShaderModule shaderModule) {
    m_device.destroyShaderModule(shaderModule);
}

vk::PipelineLayout VulkanDevice::CreatePipelineLayout(
        const std::initializer_list<vk::DescriptorSetLayout> &descriptorSetLayouts,
        const std::initializer_list<vk::PushConstantRange> &pushConstantRanges
) {
    const vk::PipelineLayoutCreateInfo createInfo(
            {},
            descriptorSetLayouts,
            pushConstantRanges
    );
    const auto [result, pipelineLayout] = m_device.createPipelineLayout(createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan pipeline layout.");
    return pipelineLayout;
}

void VulkanDevice::DestroyPipelineLayout(vk::PipelineLayout pipelineLayout) {
    m_device.destroyPipelineLayout(pipelineLayout);
}

vk::Pipeline VulkanDevice::CreatePipeline(
        vk::RenderPass renderPass,
        uint32_t subpass,
        vk::PipelineLayout pipelineLayout,
        const vk::PipelineVertexInputStateCreateInfo *vertexInput,
        const std::initializer_list<vk::PipelineShaderStageCreateInfo> &shaderStages,
        const VulkanPipelineOptions &options,
        const std::initializer_list<vk::PipelineColorBlendAttachmentState> &attachmentColorBlends
) {
    const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState(
            {},
            options.Topology
    );

    const vk::PipelineViewportStateCreateInfo viewportState(
            {},
            1,
            nullptr,
            1,
            nullptr
    );

    const vk::PipelineRasterizationStateCreateInfo rasterizationState(
            {},
            VK_FALSE,
            VK_FALSE,
            options.PolygonMode,
            options.CullMode,
            vk::FrontFace::eCounterClockwise,
            VK_FALSE,
            0.0f,
            0.0f,
            0.0f,
            1.0f
    );

    const vk::PipelineMultisampleStateCreateInfo multisampleState(
            {},
            vk::SampleCountFlagBits::e1
    );

    const vk::PipelineDepthStencilStateCreateInfo depthStencilState(
            {},
            options.DepthTestEnable,
            options.DepthWriteEnable,
            options.DepthCompareOp
    );

    const vk::PipelineColorBlendStateCreateInfo colorBlendState(
            {},
            VK_FALSE,
            vk::LogicOp::eCopy,
            attachmentColorBlends
    );

    const vk::DynamicState dynamicStates[] = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
    };
    const vk::PipelineDynamicStateCreateInfo dynamicState(
            {},
            dynamicStates
    );

    const vk::GraphicsPipelineCreateInfo createInfo(
            {},
            shaderStages,
            vertexInput,
            &inputAssemblyState,
            nullptr,
            &viewportState,
            &rasterizationState,
            &multisampleState,
            &depthStencilState,
            &colorBlendState,
            &dynamicState,
            pipelineLayout,
            renderPass,
            subpass
    );
    const auto [result, pipeline] = m_device.createGraphicsPipeline({}, createInfo);
    DebugCheckCriticalVk(result, "Failed to create Vulkan graphics pipeline.");
    return pipeline;
}

void VulkanDevice::DestroyPipeline(vk::Pipeline pipeline) {
    m_device.destroyPipeline(pipeline);
}
