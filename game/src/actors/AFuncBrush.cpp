//
// Created by andyroiiid on 12/28/2022.
//

#include "actors/AFuncBrush.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>

#include "Globals.h"

AFuncBrush::AFuncBrush(const std::vector<MapData::Brush> &brushes, BrushType type, PhysicsLayer layer)
    : m_brushes(brushes, type, layer) {
    const glm::vec3 &center = m_brushes.GetCenter();

    GetTransform().SetPosition(center);
    m_position = center;

    m_rigidbody = g_PhysicsScene->CreateStatic(physx::PxTransform{center.x, center.y, center.z});
    m_brushes.AttachToRigidActor(m_rigidbody);

    m_rigidbody->userData = this;
}

AFuncBrush::~AFuncBrush() {
    PX_RELEASE(m_rigidbody)
}

void AFuncBrush::Update(float deltaTime) {
    const float timeError = glm::min(g_PhysicsScene->GetFixedTimestep(), g_PhysicsScene->GetFixedUpdateTimeError());

    m_translationMatrix = GetTransform().SetPosition(m_position + m_velocity * timeError).GetTranslationMatrix();
}

void AFuncBrush::FixedUpdate(float fixedDeltaTime) {
    const physx::PxTransform transform = m_rigidbody->getGlobalPose();

    const glm::vec3 lastPosition = m_position;

    m_position = {transform.p.x, transform.p.y, transform.p.z};
    m_velocity = (m_position - lastPosition) / fixedDeltaTime;

    m_rotationMatrix = glm::mat4_cast(glm::quat{transform.q.w, transform.q.x, transform.q.y, transform.q.z});
}

void AFuncBrush::Draw() {
    m_brushes.Draw(m_translationMatrix * m_rotationMatrix);
}

void AFuncBrush::Move(const glm::vec3 &deltaPosition) {
    physx::PxTransform pose     = m_rigidbody->getGlobalPose();
    physx::PxVec3     &position = pose.p;
    position.x                  += deltaPosition.x;
    position.y                  += deltaPosition.y;
    position.z                  += deltaPosition.z;
    m_rigidbody->setGlobalPose(pose);
}
