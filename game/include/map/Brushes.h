//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include <glm/mat4x4.hpp>
#include <physics/PhysicsLayer.h>
#include <vulkan/VulkanMesh.h>

#include "map/MapData.h"

class PbrMaterial;

namespace physx {
class PxConvexMesh;

class PxShape;

class PxRigidActor;
} // namespace physx

enum class BrushType {
    Normal,      // mesh + collision
    NoCollision, // mesh
    NoMesh,      // collision
    Trigger      // collision(trigger)
};

class Brushes {
public:
    explicit Brushes(const std::vector<MapData::Brush> &brushes, BrushType type = BrushType::Normal, PhysicsLayer layer = PHYSICS_LAYER_0);

    ~Brushes();

    Brushes(const Brushes &) = delete;

    Brushes &operator=(const Brushes &) = delete;

    Brushes(Brushes &&) = delete;

    Brushes &operator=(Brushes &&) = delete;

    [[nodiscard]] const BrushType &GetType() const { return m_type; }

    [[nodiscard]] const glm::vec3 &GetCenter() const { return m_center; }

    void AttachToRigidActor(physx::PxRigidActor *actor);

    void Draw(const glm::mat4 &model);

private:
    void CreateMeshes(const std::vector<MapData::Brush> &brushes);

    void CreateColliders(const std::vector<MapData::Brush> &brushes, PhysicsLayer layer);

    BrushType m_type = BrushType::Normal;
    glm::vec3 m_center;

    std::vector<std::pair<VulkanMesh, PbrMaterial *>> m_meshes;

    std::vector<physx::PxConvexMesh *> m_colliders;
    std::vector<physx::PxShape *>      m_shapes;
};
