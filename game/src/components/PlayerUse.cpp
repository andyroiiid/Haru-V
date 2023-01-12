//
// Created by andyroiiid on 1/11/2023.
//

#include "components/PlayerUse.h"

#include <PxRigidActor.h>
#include <physics/PhysicsScene.h>

#include "Globals.h"
#include "actors/Actor.h"

PlayerUse::PlayerUse(Actor *owner)
    : m_owner(owner) {
}

static Actor *GetActor(const physx::PxRaycastHit &hit) {
    physx::PxRigidActor *actor = hit.actor;
    return actor ? static_cast<Actor *>(actor->userData) : nullptr;
}

physx::PxRaycastHit PlayerUse::EyeRayCast() {
    const glm::vec3 position = m_owner->GetTransform().GetPosition();
    const glm::vec3 forward  = m_owner->GetTransform().GetForwardVector();

    const physx::PxVec3 origin{position.x, position.y, position.z};
    const physx::PxVec3 unitDir{forward.x, forward.y, forward.z};
    return g_PhysicsScene->Raycast(origin, unitDir, INTERACTION_DISTANCE, PHYSICS_LAYER_0).block;
}

void PlayerUse::Update() {
    // eye raycast
    const physx::PxRaycastHit hit = EyeRayCast();

    // try to acquire current target
    Actor * const eyeTarget = GetActor(hit);

    // use input
    if (!m_prevButton) {
        if (m_currButton) {
            // press
            if (eyeTarget) {
                eyeTarget->StartUse(m_owner, hit);
            }
        }
    } else {
        if (!m_currButton) {
            // release
            if (m_prevEyeTarget) {
                m_prevEyeTarget->StopUse(m_owner);
            }
        } else {
            // hold
            if (eyeTarget == m_prevEyeTarget) {
                // still looking at the same object (might be null)
                if (eyeTarget) {
                    eyeTarget->ContinueUse(m_owner, hit);
                }
            } else {
                // target changed
                if (m_prevEyeTarget) {
                    m_prevEyeTarget->StopUse(m_owner);
                }
                if (eyeTarget) {
                    eyeTarget->StartUse(m_owner, hit);
                }
            }
        }
    }
    m_prevButton = m_currButton;

    m_prevEyeTarget = eyeTarget;
}
