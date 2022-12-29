//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include <glm/mat4x4.hpp>
#include <vulkan/VulkanMesh.h>
#include <physics/PhysicsLayer.h>

#include "map/MapData.h"

class PbrMaterial;

namespace physx {
    class PxConvexMesh;

    class PxShape;

    class PxRigidActor;
}

class Brushes {
public:
    explicit Brushes(const std::vector<MapData::Brush> &brushes, PhysicsLayer layer = PHYSICS_LAYER_0);

    ~Brushes();

    Brushes(const Brushes &) = delete;

    Brushes &operator=(const Brushes &) = delete;

    Brushes(Brushes &&) = delete;

    Brushes &operator=(Brushes &&) = delete;

    [[nodiscard]] const glm::vec3 &GetCenter() const { return m_center; }

    void AttachToRigidActor(physx::PxRigidActor *actor);

    void Draw(const glm::mat4 &model);

private:
    void CreateMeshes(const std::vector<MapData::Brush> &brushes);

    void CreateColliders(const std::vector<MapData::Brush> &brushes, PhysicsLayer layer);

    glm::vec3 m_center;

    std::vector<std::pair<VulkanMesh, PbrMaterial *>> m_meshes;

    std::vector<physx::PxConvexMesh *> m_colliders;
    std::vector<physx::PxShape *> m_shapes;
};
