//
// Created by andyroiiid on 12/21/2022.
//

#include "vulkan/VulkanUniformBufferSet.h"

#include "vulkan/VulkanBase.h"

VulkanUniformBufferSet::VulkanUniformBufferSet(VulkanBase &device, const std::initializer_list<VulkanUniformBufferInfo> &uniformBufferInfos)
        : m_device(&device) {
    const size_t numBuffering = m_device->GetNumBuffering();
    const size_t numUniformBuffers = uniformBufferInfos.size();

    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    bindings.reserve(numUniformBuffers);
    for (const auto &uniformBufferInfo: uniformBufferInfos) {
        bindings.emplace_back(
                uniformBufferInfo.Binding,
                vk::DescriptorType::eUniformBufferDynamic,
                1,
                uniformBufferInfo.Stage
        );
    }
    m_descriptorSetLayout = m_device->CreateDescriptorSetLayout(bindings);
    m_descriptorSet = m_device->AllocateDescriptorSet(m_descriptorSetLayout);

    m_uniformBufferSizes.reserve(numUniformBuffers);
    m_uniformBuffers.reserve(numUniformBuffers);
    for (const auto &uniformBufferInfo: uniformBufferInfos) {
        m_uniformBufferSizes.push_back(uniformBufferInfo.Size);

        VulkanBuffer uniformBuffer = m_device->CreateBuffer(
                numBuffering * uniformBufferInfo.Size,
                vk::BufferUsageFlagBits::eUniformBuffer,
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                VMA_MEMORY_USAGE_AUTO_PREFER_HOST
        );

        const vk::DescriptorBufferInfo bufferInfo(
                uniformBuffer.Get(),
                0,
                uniformBufferInfo.Size
        );
        const vk::WriteDescriptorSet writeDescriptorSet(
                m_descriptorSet,
                uniformBufferInfo.Binding,
                0,
                vk::DescriptorType::eUniformBufferDynamic,
                {},
                bufferInfo
        );
        m_device->WriteDescriptorSet(writeDescriptorSet);

        m_uniformBuffers.push_back(std::move(uniformBuffer));
    }

    m_dynamicOffsets.reserve(numBuffering);
    for (int i = 0; i < numBuffering; i++) {
        std::vector<uint32_t> dynamicOffsets;
        dynamicOffsets.reserve(numUniformBuffers);
        for (int j = 0; j < numUniformBuffers; j++) {
            dynamicOffsets.push_back(i * m_uniformBufferSizes[j]);
        }
        m_dynamicOffsets.push_back(dynamicOffsets);
    }
}

void VulkanUniformBufferSet::Release() {
    if (m_device) {
        m_device->FreeDescriptorSet(m_descriptorSet);
        m_device->DestroyDescriptorSetLayout(m_descriptorSetLayout);
    }

    m_device = nullptr;
    m_descriptorSetLayout = VK_NULL_HANDLE;
    m_uniformBufferSizes.clear();
    m_uniformBuffers.clear();
    m_descriptorSet = VK_NULL_HANDLE;
    m_dynamicOffsets.clear();
}

void VulkanUniformBufferSet::Swap(VulkanUniformBufferSet &other) noexcept {
    std::swap(m_device, other.m_device);
    std::swap(m_descriptorSetLayout, other.m_descriptorSetLayout);
    std::swap(m_descriptorSet, other.m_descriptorSet);
    std::swap(m_uniformBufferSizes, other.m_uniformBufferSizes);
    std::swap(m_uniformBuffers, other.m_uniformBuffers);
    std::swap(m_dynamicOffsets, other.m_dynamicOffsets);
}

void VulkanUniformBufferSet::UpdateAllBuffers(uint32_t bufferingIndex, const std::initializer_list<const void *> &allBuffersData) {
    int i = 0;
    for (const void *bufferData: allBuffersData) {
        const uint32_t size = m_uniformBufferSizes[i];
        const uint32_t offset = bufferingIndex * size;
        m_uniformBuffers[i].UploadRange(offset, size, bufferData);
        i++;
    }
}
