//
// Created by andyroiiid on 12/26/2022.
//

#pragma once

#include <map>
#include <vulkan/TextureCache.h>
#include <vulkan/VulkanBase.h>

struct PbrMaterial {
    vk::DescriptorSet DescriptorSet;
    bool              Transparent = false;
    bool              Shadow      = true;
};

class PbrMaterialCache {
public:
    PbrMaterialCache(VulkanBase &device, TextureCache &textureCache);

    ~PbrMaterialCache();

    PbrMaterialCache(const PbrMaterialCache &) = delete;

    PbrMaterialCache &operator=(const PbrMaterialCache &) = delete;

    PbrMaterialCache(PbrMaterialCache &&) = delete;

    PbrMaterialCache &operator=(PbrMaterialCache &&) = delete;

    [[nodiscard]] const vk::DescriptorSetLayout &GetDescriptorSetLayout() const { return m_textureSetLayout; }

    PbrMaterial *LoadMaterial(const std::string &filename);

private:
    VulkanBase   &m_device;
    TextureCache &m_textureCache;

    vk::DescriptorSetLayout            m_textureSetLayout;
    std::map<std::string, PbrMaterial> m_materials;
};
