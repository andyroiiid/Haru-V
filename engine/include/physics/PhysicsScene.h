//
// Created by Andrew Huang on 11/22/2022.
//

#pragma once

#include <PxQueryReport.h>
#include <characterkinematic/PxExtended.h>
#include <geometry/PxGeometry.h>

#include "physics/PhysicsLayer.h"

class PhysicsSystem;

namespace physx {
class PxPhysics;

class PxDefaultCpuDispatcher;

class PxMaterial;

class PxScene;

class PxSimulationEventCallback;

class PxController;

class PxControllerManager;

class PxRigidStatic;

class PxRigidDynamic;
} // namespace physx

class PhysicsScene {
public:
    explicit PhysicsScene(PhysicsSystem *physicsSystem);

    ~PhysicsScene();

    PhysicsScene(const PhysicsScene &) = delete;

    PhysicsScene &operator=(const PhysicsScene &) = delete;

    PhysicsScene(PhysicsScene &&) = delete;

    PhysicsScene &operator=(PhysicsScene &&) = delete;

    bool Update(float deltaTime, float timeScale);

    void SetSimulationEventCallback(physx::PxSimulationEventCallback *callback);

    [[nodiscard]] float GetFixedTimestep() const { return m_fixedTimestep; }

    [[nodiscard]] float GetFixedUpdateTimeError() const { return m_timeSinceLastTick; }

    physx::PxController *CreateController(const physx::PxVec3 &position, float radius, float height, PhysicsLayer queryLayer = PHYSICS_LAYER_1);

    physx::PxShape *CreateShape(const physx::PxGeometry &geometry, bool isExclusive);

    physx::PxRigidStatic *CreateStatic(const physx::PxTransform &transform);

    physx::PxRigidStatic *CreateStatic(
        const physx::PxTransform &transform,
        const physx::PxGeometry  &geometry,
        PhysicsLayer              queryLayer = PHYSICS_LAYER_0
    );

    physx::PxRigidDynamic *CreateDynamic(const physx::PxTransform &transform);

    physx::PxRigidDynamic *CreateDynamic(
        const physx::PxTransform &transform,
        const physx::PxGeometry  &geometry,
        PhysicsLayer              queryLayer = PHYSICS_LAYER_0,
        float                     density    = 1.0f
    );

    [[nodiscard]] physx::PxRaycastBuffer Raycast(const physx::PxVec3 &origin, const physx::PxVec3 &unitDir, float distance, PhysicsLayer layer) const;

    [[nodiscard]] physx::PxSweepBuffer Sweep(
        const physx::PxGeometry  &geometry,
        const physx::PxTransform &pose,
        const physx::PxVec3      &unitDir,
        float                     distance,
        PhysicsLayer              layer
    ) const;

private:
    physx::PxPhysics *m_physics = nullptr;

    physx::PxDefaultCpuDispatcher *m_defaultCpuDispatcher = nullptr;
    physx::PxMaterial             *m_defaultMaterial      = nullptr;
    physx::PxScene                *m_scene                = nullptr;
    physx::PxControllerManager    *m_controllerManager    = nullptr;

    float m_fixedTimestep     = 0.02f;
    float m_timeSinceLastTick = 0.0f;
};
