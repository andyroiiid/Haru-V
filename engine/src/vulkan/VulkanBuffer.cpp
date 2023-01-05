//
// Created by andyroiiid on 12/21/2022.
//

#include "vulkan/VulkanBuffer.h"

#include "vulkan/DebugVk.h"

VulkanBuffer::VulkanBuffer(
    VmaAllocator             allocator,
    vk::DeviceSize           size,
    vk::BufferUsageFlags     bufferUsage,
    VmaAllocationCreateFlags flags,
    VmaMemoryUsage           memoryUsage
)
    : m_allocator(allocator) {
    vk::BufferCreateInfo bufferCreateInfo({}, size, bufferUsage);

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.flags = flags;
    allocationCreateInfo.usage = memoryUsage;

    DebugCheckCriticalVk(
        vmaCreateBuffer(
            m_allocator,
            reinterpret_cast<const VkBufferCreateInfo *>(&bufferCreateInfo),
            &allocationCreateInfo,
            reinterpret_cast<VkBuffer *>(&m_buffer),
            &m_allocation,
            nullptr
        ),
        "Failed to create Vulkan buffer."
    );
}

void VulkanBuffer::Release() {
    if (m_allocator) {
        vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
    }

    m_allocator  = VK_NULL_HANDLE;
    m_buffer     = VK_NULL_HANDLE;
    m_allocation = VK_NULL_HANDLE;
}

void VulkanBuffer::Swap(VulkanBuffer &other) noexcept {
    std::swap(m_allocator, other.m_allocator);
    std::swap(m_buffer, other.m_buffer);
    std::swap(m_allocation, other.m_allocation);
}

void VulkanBuffer::UploadRange(size_t offset, size_t size, const void *data) const {
    void *mappedMemory = nullptr;
    DebugCheckCriticalVk(vmaMapMemory(m_allocator, m_allocation, &mappedMemory), "Failed to map Vulkan memory.");
    memcpy(static_cast<unsigned char *>(mappedMemory) + offset, data, size);
    vmaUnmapMemory(m_allocator, m_allocation);
}
