//
// Created by andyroiiid on 12/27/2022.
//

#pragma once

#include <map>

#include "vulkan/VulkanMesh.h"

class MeshCache {
public:
    explicit MeshCache(VulkanBase &device) : m_device(device) {}

    ~MeshCache() = default;

    MeshCache(const MeshCache &) = delete;

    MeshCache &operator=(const MeshCache &) = delete;

    MeshCache(MeshCache &&) = delete;

    MeshCache &operator=(MeshCache &&) = delete;

    VulkanMesh *LoadObjMesh(const std::string &filename);

private:
    VulkanBase &m_device;

    std::map<std::string, VulkanMesh> m_meshes;
};
