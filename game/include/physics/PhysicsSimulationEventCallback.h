//
// Created by andyroiiid on 1/4/2023.
//

#pragma once

#include <PxSimulationEventCallback.h>

class PhysicsSimulationEventCallback : public physx::PxSimulationEventCallback {
public:
    void onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count) override;

    void onWake(physx::PxActor **actors, physx::PxU32 count) override;

    void onSleep(physx::PxActor **actors, physx::PxU32 count) override;

    void onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs) override;

    void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) override;

    void onAdvance(const physx::PxRigidBody * const *bodyBuffer, const physx::PxTransform *poseBuffer, physx::PxU32 count) override;
};
