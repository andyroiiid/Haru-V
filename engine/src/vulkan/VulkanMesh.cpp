//
// Created by andyroiiid on 12/21/2022.
//

#include "vulkan/VulkanMesh.h"

#include "vulkan/VulkanBase.h"

VulkanMesh::VulkanMesh(VulkanBase &device, size_t vertexCount, size_t vertexSize, const void *data) {
    const vk::DeviceSize size = vertexCount * vertexSize;

    VulkanBuffer uploadBuffer = device.CreateBuffer(
            size,
            vk::BufferUsageFlagBits::eTransferSrc,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_HOST
    );
    uploadBuffer.Upload(size, data);

    VulkanBuffer vertexBuffer = device.CreateBuffer(
            size,
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            0,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    );

    device.ImmediateSubmit([size, &uploadBuffer, &vertexBuffer](vk::CommandBuffer cmd) {
        const vk::BufferCopy copy(0, 0, size);
        cmd.copyBuffer(uploadBuffer.Get(), vertexBuffer.Get(), 1, &copy);
    });

    m_vertexBuffer = std::move(vertexBuffer);
    m_vertexCount = vertexCount;
}

void VulkanMesh::Release() {
    m_vertexBuffer = {};
    m_vertexCount = 0;
}

void VulkanMesh::Swap(VulkanMesh &other) noexcept {
    std::swap(m_vertexBuffer, other.m_vertexBuffer);
    std::swap(m_vertexCount, other.m_vertexCount);
}

void VulkanMesh::BindAndDraw(vk::CommandBuffer commandBuffer) const {
    const vk::DeviceSize offset = 0;
    commandBuffer.bindVertexBuffers(0, 1, &m_vertexBuffer.Get(), &offset);
    commandBuffer.draw(m_vertexCount, 1, 0, 0);
}
