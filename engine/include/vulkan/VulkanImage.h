//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

class VulkanImage {
public:
    VulkanImage() = default;

    VulkanImage(
            VmaAllocator allocator,
            vk::Format format,
            const vk::Extent2D &extent,
            vk::ImageUsageFlags imageUsage,
            VmaAllocationCreateFlags flags,
            VmaMemoryUsage memoryUsage
    );

    ~VulkanImage() {
        Release();
    }

    VulkanImage(const VulkanImage &) = delete;

    VulkanImage &operator=(const VulkanImage &) = delete;

    VulkanImage(VulkanImage &&other) noexcept {
        Swap(other);
    }

    VulkanImage &operator=(VulkanImage &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(VulkanImage &other) noexcept;

    [[nodiscard]] const vk::Image &Get() const { return m_image; }

private:
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    vk::Image m_image = VK_NULL_HANDLE;
    VmaAllocation m_allocation = VK_NULL_HANDLE;
};
