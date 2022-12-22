//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include "vulkan/VulkanBuffer.h"

class VulkanBase;

struct VulkanUniformBufferInfo {
    uint32_t Binding;
    vk::ShaderStageFlags Stage;
    size_t Size;
};

class VulkanUniformBufferSet {
public:
    VulkanUniformBufferSet() = default;

    VulkanUniformBufferSet(VulkanBase &device, const std::initializer_list<VulkanUniformBufferInfo> &uniformBufferInfos);

    ~VulkanUniformBufferSet() {
        Release();
    }

    VulkanUniformBufferSet(const VulkanUniformBufferSet &) = delete;

    VulkanUniformBufferSet &operator=(const VulkanUniformBufferSet &) = delete;

    VulkanUniformBufferSet(VulkanUniformBufferSet &&other) noexcept {
        Swap(other);
    }

    VulkanUniformBufferSet &operator=(VulkanUniformBufferSet &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(VulkanUniformBufferSet &other) noexcept;

    [[nodiscard]] const vk::DescriptorSetLayout &GetDescriptorSetLayout() const { return m_descriptorSetLayout; }

    [[nodiscard]] const vk::DescriptorSet &GetDescriptorSet() const { return m_descriptorSet; }

    [[nodiscard]] const std::vector<uint32_t> &GetDynamicOffsets(uint32_t bufferingIndex) const { return m_dynamicOffsets[bufferingIndex]; }

    void UpdateAllBuffers(uint32_t bufferingIndex, const std::initializer_list<const void *> &allBuffersData);

private:
    VulkanBase *m_device = nullptr;

    vk::DescriptorSetLayout m_descriptorSetLayout;
    vk::DescriptorSet m_descriptorSet;
    std::vector<uint32_t> m_uniformBufferSizes;
    std::vector<VulkanBuffer> m_uniformBuffers;
    std::vector<std::vector<uint32_t>> m_dynamicOffsets;
};
