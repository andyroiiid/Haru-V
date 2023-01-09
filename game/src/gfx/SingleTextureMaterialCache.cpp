//
// Created by andyroiiid on 1/8/2023.
//

#include "gfx/SingleTextureMaterialCache.h"

#include <core/Debug.h>

#include "JsonFiles.h"

SingleTextureMaterialCache::SingleTextureMaterialCache(VulkanBase &device, TextureCache &textureCache)
    : m_device(device)
    , m_textureCache(textureCache) {
    vk::DescriptorSetLayoutBinding bindings[]{
        {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
    };
    m_textureSetLayout = m_device.CreateDescriptorSetLayout(bindings);
}

SingleTextureMaterialCache::~SingleTextureMaterialCache() {
    for (const auto &[name, material]: m_materials) {
        m_device.FreeDescriptorSet(material.DescriptorSet);
    }
    m_device.DestroyDescriptorSetLayout(m_textureSetLayout);
}

SingleTextureMaterial *SingleTextureMaterialCache::LoadMaterial(const std::string &filename) {
    auto pair = m_materials.find(filename);
    if (pair == m_materials.end()) {
        DebugInfo("Caching texture material {}.", filename);

        const vk::DescriptorSet textureSet = m_device.AllocateDescriptorSet(m_textureSetLayout);

        m_textureCache.LoadTexture(filename)->BindToDescriptorSet(textureSet, 0);

        const SingleTextureMaterial material{textureSet};

        pair = m_materials.emplace(filename, material).first;
    }
    return &pair->second;
}
