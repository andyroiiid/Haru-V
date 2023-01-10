//
// Created by andyroiiid on 1/4/2023.
//

#pragma once

#include "actors/Actor.h"

#include <foundation/PxVec3.h>

namespace physx {
class PxRigidDynamic;
class PxMaterial;
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

    void StartUse(Actor *user, const physx::PxRaycastHit &hit) override;

    void ContinueUse(Actor *user, const physx::PxRaycastHit &hit) override;

    void StopUse(Actor *user) override;

private:
    void EnableDamping();

    void DisableDamping();

    physx::PxRigidDynamic *m_rigidbody;
    physx::PxMaterial     *m_physicsMaterial;

    glm::mat4 m_translationMatrix{1.0f};

    glm::vec3 m_position{};
    glm::vec3 m_velocity{};

    glm::quat m_rotation{};
    glm::quat m_currentRotation{};

    VulkanMesh  *m_mesh;
    PbrMaterial *m_material;

    bool          m_beingPushed = false;
    physx::PxVec3 m_pushPosition;
    physx::PxVec3 m_pushForce;
};
