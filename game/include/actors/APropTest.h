//
// Created by andyroiiid on 12/27/2022.
//

#pragma once

#include <math/Transform.h>

#include "actors/Actor.h"

class VulkanMesh;

class PbrMaterial;

class APropTest : public Actor {
public:
    DEFINE_ACTOR_CLASS(APropTest)

    APropTest(const std::string &meshName, const std::string &materialName, const glm::vec3 &position);

    void Draw() override;

private:
    Transform m_transform;

    VulkanMesh *m_mesh;
    PbrMaterial *m_material;
};
