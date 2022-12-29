//
// Created by andyroiiid on 12/26/2022.
//

#include "vulkan/TextureCache.h"

#include "core/Debug.h"
#include "file/ImageFile.h"

VulkanTexture *TextureCache::LoadTexture(const std::string &filename, vk::Filter filter, vk::SamplerAddressMode addressMode) {
    auto pair = m_textures.find(filename);
    if (pair == m_textures.end()) {
        DebugInfo("Caching texture {}.", filename);
        const ImageFile image(filename);
        pair = m_textures.emplace(
                filename,
                VulkanTexture(m_device, image.GetWidth(), image.GetHeight(), image.GetData(), filter, addressMode)
        ).first;
    }
    return &pair->second;
}
