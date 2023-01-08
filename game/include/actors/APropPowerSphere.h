//
// Created by andyroiiid on 1/4/2023.
//

#pragma once

#include "actors/Actor.h"

namespace physx {
class PxRigidDynamic;
} // namespace physx

class VulkanMesh;

class PbrMaterial;

class APropPowerSphere : public Actor {
public:
    DEFINE_ACTOR_CLASS(APropPowerSphere)

    explicit APropPowerSphere(const glm::vec3 &position);

    ~APropPowerSphere() override;

    void Update(float deltaTime) override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw() override;

    void Use(APlayer *player, const physx::PxRaycastHit &hit) override;

private:
    physx::PxRigidDynamic *m_rigidbody;

    glm::mat4 m_translationMatrix{1.0f};

    glm::vec3 m_position{};
    glm::vec3 m_velocity{};

    glm::quat m_rotation{};
    glm::quat m_currentRotation{};

    VulkanMesh  *m_mesh;
    PbrMaterial *m_material;
};
