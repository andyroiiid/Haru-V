//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include <vulkan/VulkanMesh.h>

#include "actors/Actor.h"
#include "map/MapData.h"

class PbrMaterial;

class AWorldSpawn final : public Actor {
public:
    DEFINE_ACTOR_CLASS(AWorldSpawn)

    explicit AWorldSpawn(const std::vector<MapData::Brush> &brushes);

    void Draw() final;

private:
    std::vector<std::pair<VulkanMesh, PbrMaterial *>> m_meshes;
};
