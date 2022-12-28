//
// Created by andyroiiid on 12/22/2022.
//
#include "vulkan/VulkanTexture.h"

#include "vulkan/VulkanBase.h"

VulkanTexture::VulkanTexture(
        VulkanBase &device,
        uint32_t width,
        uint32_t height,
        const unsigned char *data,
        vk::Filter filter,
        vk::SamplerAddressMode addressMode
) : m_device(&device) {
    constexpr size_t PIXEL_SIZE = sizeof(unsigned char) * 4;
    CreateImage(width, height, width * height * PIXEL_SIZE, data, vk::Format::eR8G8B8A8Unorm);
    CreateImageView(vk::Format::eR8G8B8A8Unorm);
    CreateSampler(filter, addressMode);
}

VulkanTexture::VulkanTexture(
        VulkanBase &device,
        uint32_t width,
        uint32_t height,
        const float *data,
        vk::Filter filter,
        vk::SamplerAddressMode addressMode
) : m_device(&device) {
    constexpr size_t PIXEL_SIZE = sizeof(float) * 4;
    CreateImage(width, height, width * height * PIXEL_SIZE, data, vk::Format::eR32G32B32A32Sfloat);
    CreateImageView(vk::Format::eR32G32B32A32Sfloat);
    CreateSampler(filter, addressMode);
}

void VulkanTexture::CreateImage(uint32_t width, uint32_t height, vk::DeviceSize size, const void *data, vk::Format format) {
    VulkanBuffer uploadBuffer = m_device->CreateBuffer(
            size,
            vk::BufferUsageFlagBits::eTransferSrc,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_HOST
    );
    uploadBuffer.Upload(size, data);

    VulkanImage image = m_device->CreateImage(
            format,
            VkExtent2D{width, height},
            vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
            0,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    );

    const vk::Extent3D extent{width, height, 1};

    m_device->ImmediateSubmit([extent, &uploadBuffer, &image](vk::CommandBuffer cmd) {
        vk::ImageMemoryBarrier imageMemoryBarrier(
                {},
                vk::AccessFlagBits::eTransferWrite,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                {},
                {},
                image.Get(),
                {
                        vk::ImageAspectFlagBits::eColor,
                        0,
                        1,
                        0,
                        1
                }
        );
        cmd.pipelineBarrier(
                vk::PipelineStageFlagBits::eTopOfPipe,
                vk::PipelineStageFlagBits::eTransfer,
                {},
                {},
                {},
                imageMemoryBarrier
        );

        const vk::BufferImageCopy imageCopy(
                0,
                0,
                0,
                {
                        vk::ImageAspectFlagBits::eColor,
                        0,
                        0,
                        1
                },
                {0, 0, 0},
                extent
        );
        cmd.copyBufferToImage(uploadBuffer.Get(), image.Get(), vk::ImageLayout::eTransferDstOptimal, imageCopy);

        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        imageMemoryBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        imageMemoryBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        cmd.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eFragmentShader,
                {},
                {},
                {},
                imageMemoryBarrier
        );
    });

    m_image = std::move(image);
}

void VulkanTexture::CreateImageView(vk::Format format) {
    m_imageView = m_device->CreateImageView(m_image.Get(), format, vk::ImageAspectFlagBits::eColor);
}

void VulkanTexture::CreateSampler(vk::Filter filter, vk::SamplerAddressMode addressMode) {
    m_sampler = m_device->CreateSampler(filter, addressMode);
}

void VulkanTexture::Release() {
    if (m_device) {
        m_device->DestroySampler(m_sampler);
        m_device->DestroyImageView(m_imageView);
    }

    m_device = nullptr;
    m_image = {};
    m_imageView = VK_NULL_HANDLE;
    m_sampler = VK_NULL_HANDLE;
}

void VulkanTexture::Swap(VulkanTexture &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_image, other.m_image);
    std::swap(m_imageView, other.m_imageView);
    std::swap(m_sampler, other.m_sampler);
}

void VulkanTexture::BindToDescriptorSet(vk::DescriptorSet descriptorSet, uint32_t binding) {
    m_device->WriteCombinedImageSamplerToDescriptorSet(
            m_sampler,
            m_imageView,
            descriptorSet,
            binding
    );
}
