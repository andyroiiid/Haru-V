//
// Created by andyroiiid on 12/26/2022.
//

#include "vulkan/TextureCache.h"

#include "core/Debug.h"
#include "file/ImageFile.h"
#include "file/HdrImageFile.h"

VulkanTexture *TextureCache::LoadTexture(const std::string &filename) {
    auto pair = m_textures.find(filename);
    if (pair == m_textures.end()) {
        DebugInfo("Caching texture {}.", filename);
        const ImageFile image(filename);
        pair = m_textures.emplace(
                filename,
                VulkanTexture(m_device, image.GetWidth(), image.GetHeight(), image.GetData())
        ).first;
    }
    return &pair->second;
}

VulkanTexture *TextureCache::LoadHdrTexture(const std::string &filename) {
    auto pair = m_hdrTextures.find(filename);
    if (pair == m_hdrTextures.end()) {
        DebugInfo("Caching hdr texture {}.", filename);
        const HdrImageFile image(filename);
        pair = m_hdrTextures.emplace(
                filename,
                VulkanTexture(m_device, image.GetWidth(), image.GetHeight(), image.GetData())
        ).first;
    }
    return &pair->second;
}
