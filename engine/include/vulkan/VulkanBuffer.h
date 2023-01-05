//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

class VulkanBuffer {
public:
    VulkanBuffer() = default;

    VulkanBuffer(
        VmaAllocator             allocator,
        vk::DeviceSize           size,
        vk::BufferUsageFlags     bufferUsage,
        VmaAllocationCreateFlags flags,
        VmaMemoryUsage           memoryUsage
    );

    ~VulkanBuffer() { Release(); }

    VulkanBuffer(const VulkanBuffer &) = delete;

    VulkanBuffer &operator=(const VulkanBuffer &) = delete;

    VulkanBuffer(VulkanBuffer &&other) noexcept { Swap(other); }

    VulkanBuffer &operator=(VulkanBuffer &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(VulkanBuffer &other) noexcept;

    void UploadRange(size_t offset, size_t size, const void *data) const;

    void Upload(size_t size, const void *data) const { UploadRange(0, size, data); }

    [[nodiscard]] const vk::Buffer &Get() const { return m_buffer; }

private:
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    vk::Buffer    m_buffer;
    VmaAllocation m_allocation = VK_NULL_HANDLE;
};
