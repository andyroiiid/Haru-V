//
// Created by andyroiiid on 12/18/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#include "vulkan/VulkanBuffer.h"
#include "vulkan/VulkanImage.h"

struct GLFWwindow;

struct VulkanPipelineOptions {
    vk::PrimitiveTopology Topology = vk::PrimitiveTopology::eTriangleList;
    vk::PolygonMode PolygonMode = vk::PolygonMode::eFill;
    vk::CullModeFlags CullMode = vk::CullModeFlagBits::eBack;
    vk::Bool32 DepthTestEnable = VK_TRUE;
    vk::Bool32 DepthWriteEnable = VK_TRUE;
    vk::CompareOp DepthCompareOp = vk::CompareOp::eLess;
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

    VulkanBuffer CreateBuffer(
            vk::DeviceSize size,
            vk::BufferUsageFlags bufferUsage,
            VmaAllocationCreateFlags flags,
            VmaMemoryUsage memoryUsage
    ) {
        return {m_allocator, size, bufferUsage, flags, memoryUsage};
    }

    VulkanImage CreateImage(
            vk::Format format,
            const vk::Extent2D &extent,
            vk::ImageUsageFlags imageUsage,
            VmaAllocationCreateFlags flags,
            VmaMemoryUsage memoryUsage,
            uint32_t layers = 1
    ) {
        return {m_allocator, format, extent, imageUsage, flags, memoryUsage, layers};
    }

    vk::ImageView CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectMask, uint32_t layers = 1);

    void DestroyImageView(vk::ImageView imageView);

    vk::Sampler CreateSampler(
            vk::Filter filter,
            vk::SamplerAddressMode addressMode,
            vk::Bool32 compareEnable = VK_FALSE,
            vk::CompareOp compareOp = vk::CompareOp::eNever
    );

    void DestroySampler(vk::Sampler sampler);

    vk::RenderPass CreateRenderPass(
            const std::initializer_list<vk::Format> &colorAttachmentFormats,
            vk::Format depthStencilAttachmentFormat = vk::Format::eUndefined,
            bool shaderReadsDepth = false,
            bool forPresent = false
    );

    void DestroyRenderPass(vk::RenderPass renderPass);

    vk::Framebuffer CreateFramebuffer(
            vk::RenderPass renderPass,
            const vk::ArrayProxyNoTemporaries<vk::ImageView> &attachments,
            const vk::Extent2D &extent,
            uint32_t layers = 1
    );

    void DestroyFramebuffer(vk::Framebuffer framebuffer);

    vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::ArrayProxyNoTemporaries<vk::DescriptorSetLayoutBinding> &bindings);

    void DestroyDescriptorSetLayout(vk::DescriptorSetLayout descriptorSetLayout);

    vk::DescriptorSet AllocateDescriptorSet(vk::DescriptorSetLayout descriptorSetLayout);

    void FreeDescriptorSet(vk::DescriptorSet descriptorSet);

    void WriteCombinedImageSamplerToDescriptorSet(vk::Sampler sampler, vk::ImageView imageView, vk::DescriptorSet descriptorSet, uint32_t binding);

    void WriteDynamicUniformBufferToDescriptorSet(vk::Buffer buffer, vk::DeviceSize size, vk::DescriptorSet descriptorSet, uint32_t binding);

    vk::PipelineLayout CreatePipelineLayout(
            const std::initializer_list<vk::DescriptorSetLayout> &descriptorSetLayouts,
            const std::initializer_list<vk::PushConstantRange> &pushConstantRanges
    );

    void DestroyPipelineLayout(vk::PipelineLayout pipelineLayout);

    vk::ShaderModule CreateShaderModule(const std::vector<uint32_t> &spirv);

    void DestroyShaderModule(vk::ShaderModule shaderModule);

    vk::Pipeline CreatePipeline(
            vk::PipelineLayout pipelineLayout,
            const vk::PipelineVertexInputStateCreateInfo *vertexInput,
            const vk::ArrayProxyNoTemporaries<vk::PipelineShaderStageCreateInfo> &shaderStages,
            const VulkanPipelineOptions &options,
            const std::initializer_list<vk::PipelineColorBlendAttachmentState> &attachmentColorBlends,
            vk::RenderPass renderPass,
            uint32_t subpass
    );

    void DestroyPipeline(vk::Pipeline pipeline);

protected:
    void CreateInstance();

    void CreateDebugMessenger();

    void PickPhysicalDevice();

    void CreateDevice();

    void SubmitToGraphicsQueue(const vk::SubmitInfo &submitInfo, vk::Fence fence);

    void CreateCommandPool();

    void CreateDescriptorPool();

    void WriteDescriptorSet(const vk::WriteDescriptorSet &writeDescriptorSet);

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

    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;

    vk::PhysicalDevice m_physicalDevice;
    uint32_t m_graphicsQueueFamily = 0;
    vk::Device m_device;
    vk::Queue m_graphicsQueue;

    vk::CommandPool m_commandPool;

    vk::DescriptorPool m_descriptorPool;

    VmaAllocator m_allocator = VK_NULL_HANDLE;
};

void BeginCommandBuffer(vk::CommandBuffer commandBuffer, vk::CommandBufferUsageFlags flags = {});

void EndCommandBuffer(vk::CommandBuffer commandBuffer);
