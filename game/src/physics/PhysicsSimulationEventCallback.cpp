//
// Created by andyroiiid on 1/4/2023.
//
#include "physics/PhysicsSimulationEventCallback.h"

#include <PxActor.h>
#include <core/Debug.h>

#include "actors/Actor.h"

void PhysicsSimulationEventCallback::onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count) {
}

void PhysicsSimulationEventCallback::onWake(physx::PxActor **actors, physx::PxU32 count) {
}

void PhysicsSimulationEventCallback::onSleep(physx::PxActor **actors, physx::PxU32 count) {
}

void PhysicsSimulationEventCallback::onContact(
    const physx::PxContactPairHeader &pairHeader,
    const physx::PxContactPair       *pairs,
    physx::PxU32                      nbPairs
) {
}

void PhysicsSimulationEventCallback::onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count) {
    for (int i = 0; i < count; i++) {
        const physx::PxTriggerPair &pair = pairs[i];
        if (pair.flags == physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER || pair.flags == physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER) {
            // ignore events caused by shape removal
            continue;
        }

        auto triggerActor = static_cast<Actor *>(pair.triggerActor->userData);
        auto otherActor   = static_cast<Actor *>(pair.otherActor->userData);

        switch (pair.status) {
        case physx::PxPairFlag::eNOTIFY_TOUCH_FOUND:
            triggerActor->OnTriggerEnter(otherActor);
            break;
        case physx::PxPairFlag::eNOTIFY_TOUCH_LOST:
            triggerActor->OnTriggerExit(otherActor);
            break;
        default:
            DebugError("Invalid trigger pair status {}", pair.status);
            break;
        }
    }
}

void PhysicsSimulationEventCallback::onAdvance(
    const physx::PxRigidBody * const *bodyBuffer,
    const physx::PxTransform         *poseBuffer,
    physx::PxU32                      count
) {
}
