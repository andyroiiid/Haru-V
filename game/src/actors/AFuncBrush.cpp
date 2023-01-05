//
// Created by andyroiiid on 12/28/2022.
//

#include "actors/AFuncBrush.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>

#include "Globals.h"

AFuncBrush::AFuncBrush(const std::vector<MapData::Brush> &brushes)
    : m_brushes(brushes) {
    const glm::vec3 &center = m_brushes.GetCenter();

    GetTransform().SetPosition(center);

    m_rigidbody = g_PhysicsScene->CreateStatic(physx::PxTransform{center.x, center.y, center.z});
    m_brushes.AttachToRigidActor(m_rigidbody);

    m_rigidbody->userData = this;
}

AFuncBrush::~AFuncBrush() {
    PX_RELEASE(m_rigidbody)
}

void AFuncBrush::FixedUpdate(float fixedDeltaTime) {
    const physx::PxTransform transform = m_rigidbody->getGlobalPose();
    const glm::vec3          position{transform.p.x, transform.p.y, transform.p.z};
    const glm::quat          rotation{transform.q.w, transform.q.x, transform.q.y, transform.q.z};
    m_modelMatrix = GetTransform().SetPosition(position).GetTranslationMatrix() * mat4_cast(rotation);
}

void AFuncBrush::Draw() {
    m_brushes.Draw(m_modelMatrix);
}

void AFuncBrush::Move(const glm::vec3 &deltaPosition) {
    physx::PxTransform pose     = m_rigidbody->getGlobalPose();
    physx::PxVec3     &position = pose.p;
    position.x                  += deltaPosition.x;
    position.y                  += deltaPosition.y;
    position.z                  += deltaPosition.z;
    m_rigidbody->setGlobalPose(pose);
}
