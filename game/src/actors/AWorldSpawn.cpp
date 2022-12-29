//
// Created by andyroiiid on 12/28/2022.
//

#include "actors/AWorldSpawn.h"

#include <glm/gtc/matrix_transform.hpp>
#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>

#include "Globals.h"

AWorldSpawn::AWorldSpawn(const std::vector<MapData::Brush> &brushes)
        : m_brushes(brushes) {
    const glm::vec3 &center = m_brushes.GetCenter();
    m_rigidbody = g_PhysicsScene->CreateStatic(physx::PxTransform{center.x, center.y, center.z});
    m_brushes.AttachToRigidActor(m_rigidbody);
    m_transform = glm::translate(glm::mat4(1.0f), center);
}

AWorldSpawn::~AWorldSpawn() {
    PX_RELEASE(m_rigidbody)
}

void AWorldSpawn::Draw() {
    m_brushes.Draw(m_transform);
}
