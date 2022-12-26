//
// Created by andyroiiid on 12/22/2022.
//

#pragma once

#include "vulkan/VulkanImage.h"

class VulkanBase;

class VulkanTexture {
public:
    VulkanTexture() = default;

    // R8G8B8A8Unorm
    VulkanTexture(VulkanBase &device, uint32_t width, uint32_t height, const unsigned char *data);

    // R32G32B32A32Sfloat
    VulkanTexture(VulkanBase &device, uint32_t width, uint32_t height, const float *data);

    ~VulkanTexture() {
        Release();
    }

    VulkanTexture(const VulkanTexture &) = delete;

    VulkanTexture &operator=(const VulkanTexture &) = delete;

    VulkanTexture(VulkanTexture &&other) noexcept {
        Swap(other);
    }

    VulkanTexture &operator=(VulkanTexture &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(VulkanTexture &other) noexcept;

    void BindToDescriptorSet(vk::DescriptorSet descriptorSet, uint32_t binding);

private:
    void CreateImage(uint32_t width, uint32_t height, vk::DeviceSize size, const void *data, vk::Format format);

    void CreateImageView(vk::Format format);

    void CreateSampler();

    VulkanBase *m_device = nullptr;

    VulkanImage m_image;
    vk::ImageView m_imageView;
    vk::Sampler m_sampler;
};
