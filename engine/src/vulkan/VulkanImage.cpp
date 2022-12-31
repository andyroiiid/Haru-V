//
// Created by andyroiiid on 12/21/2022.
//

#include "vulkan/VulkanImage.h"

#include "vulkan/DebugVk.h"

VulkanImage::VulkanImage(
        VmaAllocator allocator,
        vk::Format format,
        const vk::Extent2D &extent,
        vk::ImageUsageFlags imageUsage,
        VmaAllocationCreateFlags flags,
        VmaMemoryUsage memoryUsage,
        uint32_t layers
) : m_allocator(allocator) {
    vk::ImageCreateInfo imageCreateInfo(
            {},
            vk::ImageType::e2D,
            format,
            {extent.width, extent.height, 1},
            1,
            layers,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            imageUsage
    );

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.flags = flags;
    allocationCreateInfo.usage = memoryUsage;

    DebugCheckCriticalVk(
            vmaCreateImage(
                    m_allocator,
                    reinterpret_cast<const VkImageCreateInfo *>(&imageCreateInfo),
                    &allocationCreateInfo,
                    reinterpret_cast<VkImage *>(&m_image),
                    &m_allocation,
                    nullptr
            ),
            "Failed to create Vulkan 2d image."
    );
}

void VulkanImage::Release() {
    if (m_allocator) {
        vmaDestroyImage(m_allocator, m_image, m_allocation);
    }

    m_allocator = VK_NULL_HANDLE;
    m_image = VK_NULL_HANDLE;
    m_allocation = VK_NULL_HANDLE;
}

void VulkanImage::Swap(VulkanImage &other) noexcept {
    std::swap(m_allocator, other.m_allocator);
    std::swap(m_image, other.m_image);
    std::swap(m_allocation, other.m_allocation);
}
