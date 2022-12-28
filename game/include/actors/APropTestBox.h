//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include <glm/vec3.hpp>
#include <vulkan/VulkanMesh.h>

#include "actors/Actor.h"

class PbrMaterial;

class APropTestBox : public Actor {
public:
    DEFINE_ACTOR_CLASS(APropTestBox)

    APropTestBox(const std::string &materialName, const glm::vec3 &min, const glm::vec3 &max);

    void Draw() override;

private:
    VulkanMesh m_mesh;
    PbrMaterial *m_material;
};
