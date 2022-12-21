//
// Created by andyroiiid on 12/18/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

struct GLFWwindow;

struct VulkanPipelineOptions {
    vk::PrimitiveTopology Topology = vk::PrimitiveTopology::eTriangleList;
    vk::PolygonMode PolygonMode = vk::PolygonMode::eFill;
    vk::CullModeFlags CullMode = vk::CullModeFlagBits::eBack;
    vk::Bool32 DepthTestEnable = VK_FALSE;
    vk::Bool32 DepthWriteEnable = VK_FALSE;
    vk::CompareOp DepthCompareOp = vk::CompareOp::eAlways;
};

class VulkanDevice {
public:
    VulkanDevice();

    ~VulkanDevice();

    VulkanDevice(const VulkanDevice &) = delete;

    VulkanDevice &operator=(const VulkanDevice &) = delete;

    VulkanDevice(VulkanDevice &&) = delete;

    VulkanDevice &operator=(VulkanDevice &&) = delete;

    void WaitIdle();

    vk::RenderPass CreateRenderPass(
            const std::initializer_list<vk::Format> &colorAttachmentFormats,
            vk::Format depthStencilAttachmentFormat = vk::Format::eUndefined,
            bool forPresent = false
    );

    void DestroyRenderPass(vk::RenderPass renderPass);

    vk::Framebuffer CreateFramebuffer(
            vk::RenderPass renderPass,
            const std::initializer_list<vk::ImageView> &attachments,
            const vk::Extent2D &extent
    );

    void DestroyFramebuffer(vk::Framebuffer framebuffer);

    vk::ShaderModule CreateShaderModule(const std::vector<uint32_t> &spirv);

    void DestroyShaderModule(vk::ShaderModule shaderModule);

    vk::PipelineLayout CreatePipelineLayout(
            const std::initializer_list<vk::DescriptorSetLayout> &descriptorSetLayouts,
            const std::initializer_list<vk::PushConstantRange> &pushConstantRanges
    );

    void DestroyPipelineLayout(vk::PipelineLayout pipelineLayout);

    vk::Pipeline CreatePipeline(
            vk::RenderPass renderPass,
            uint32_t subpass,
            vk::PipelineLayout pipelineLayout,
            const vk::PipelineVertexInputStateCreateInfo *vertexInput,
            const std::initializer_list<vk::PipelineShaderStageCreateInfo> &shaderStages,
            const VulkanPipelineOptions &options,
            const std::initializer_list<vk::PipelineColorBlendAttachmentState> &attachmentColorBlends
    );

    void DestroyPipeline(vk::Pipeline pipeline);

protected:
    void CreateInstance();

    void CreateDebugMessenger();

    void PickPhysicalDevice();

    void CreateDevice();

    void CreateCommandPool();

    void CreateAllocator();

    vk::Fence CreateFence(vk::FenceCreateFlags flags = {});

    void WaitAndResetFence(vk::Fence fence, uint64_t timeout = 100'000'000);

    vk::Semaphore CreateSemaphore();

    vk::CommandBuffer AllocateCommandBuffer();

    vk::SurfaceKHR CreateSurface(GLFWwindow *window);

    vk::SwapchainKHR CreateSwapchain(
            vk::SurfaceKHR surface,
            uint32_t imageCount,
            vk::Format imageFormat,
            vk::ColorSpaceKHR imageColorSpace,
            const vk::Extent2D &imageExtent,
            vk::SurfaceTransformFlagBitsKHR preTransform,
            vk::PresentModeKHR presentMode,
            vk::SwapchainKHR oldSwapchain
    );

    vk::ImageView CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectMask);

    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;

    vk::PhysicalDevice m_physicalDevice;
    uint32_t m_graphicsQueueFamily = 0;
    vk::Device m_device;
    vk::Queue m_graphicsQueue;

    vk::CommandPool m_commandPool;

    VmaAllocator m_allocator = VK_NULL_HANDLE;
};
