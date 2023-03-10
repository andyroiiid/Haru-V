//
// Created by andyroiiid on 12/26/2022.
//

#include "gfx/PbrMaterialCache.h"

#include <core/Debug.h>

#include "JsonFiles.h"

PbrMaterialCache::PbrMaterialCache(VulkanBase &device, TextureCache &textureCache)
    : m_device(device)
    , m_textureCache(textureCache) {
    vk::DescriptorSetLayoutBinding bindings[]{
        {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
        {1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
        {2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
        {3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_textureSetLayout = m_device.CreateDescriptorSetLayout(bindings);
}

PbrMaterialCache::~PbrMaterialCache() {
    for (const auto &[name, material]: m_materials) {
        m_device.FreeDescriptorSet(material.DescriptorSet);
    }
    m_device.DestroyDescriptorSetLayout(m_textureSetLayout);
}

PbrMaterial *PbrMaterialCache::LoadMaterial(const std::string &filename) {
    auto pair = m_materials.find(filename);
    if (pair == m_materials.end()) {
        DebugInfo("Caching material {}.", filename);

        const vk::DescriptorSet textureSet = m_device.AllocateDescriptorSet(m_textureSetLayout);

        const PbrMaterialConfig config(filename);
        m_textureCache.LoadTexture(config.Albedo)->BindToDescriptorSet(textureSet, 0);
        m_textureCache.LoadTexture(config.Normal)->BindToDescriptorSet(textureSet, 1);
        m_textureCache.LoadTexture(config.MRA)->BindToDescriptorSet(textureSet, 2);
        m_textureCache.LoadTexture(config.Emissive)->BindToDescriptorSet(textureSet, 3);

        const PbrMaterial material{
            textureSet, //
            config.Transparent,
            config.Shadow};

        pair = m_materials.emplace(filename, material).first;
    }
    return &pair->second;
}
