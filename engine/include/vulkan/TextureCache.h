//
// Created by andyroiiid on 12/26/2022.
//

#pragma once

#include <map>

#include "vulkan/VulkanTexture.h"

class TextureCache {
public:
    explicit TextureCache(VulkanBase &device) : m_device(device) {}

    ~TextureCache() = default;

    TextureCache(const TextureCache &) = delete;

    TextureCache &operator=(const TextureCache &) = delete;

    TextureCache(TextureCache &&) = delete;

    TextureCache &operator=(TextureCache &&) = delete;

    VulkanTexture *LoadTexture(const std::string &filename);

    VulkanTexture *LoadHdrTexture(const std::string &filename);

private:
    VulkanBase &m_device;

    std::map<std::string, VulkanTexture> m_textures;
    std::map<std::string, VulkanTexture> m_hdrTextures;
};
