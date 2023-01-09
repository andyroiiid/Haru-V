//
// Created by andyroiiid on 1/8/2023.
//

#pragma once

#include <map>
#include <vulkan/TextureCache.h>
#include <vulkan/VulkanBase.h>

struct SingleTextureMaterial {
    vk::DescriptorSet DescriptorSet;
};

class SingleTextureMaterialCache {
public:
    SingleTextureMaterialCache(VulkanBase &device, TextureCache &textureCache);

    ~SingleTextureMaterialCache();

    SingleTextureMaterialCache(const SingleTextureMaterialCache &) = delete;

    SingleTextureMaterialCache &operator=(const SingleTextureMaterialCache &) = delete;

    SingleTextureMaterialCache(SingleTextureMaterialCache &&) = delete;

    SingleTextureMaterialCache &operator=(SingleTextureMaterialCache &&) = delete;

    [[nodiscard]] const vk::DescriptorSetLayout &GetDescriptorSetLayout() const { return m_textureSetLayout; }

    SingleTextureMaterial *LoadMaterial(const std::string &filename);

private:
    VulkanBase   &m_device;
    TextureCache &m_textureCache;

    vk::DescriptorSetLayout                      m_textureSetLayout;
    std::map<std::string, SingleTextureMaterial> m_materials;
};
