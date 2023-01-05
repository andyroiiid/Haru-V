//
// Created by andyroiiid on 12/28/2022.
//

#include "actors/AFuncPhys.h"

#include <PxRigidDynamic.h>
#include <extensions/PxRigidBodyExt.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>

#include "Globals.h"

AFuncPhys::AFuncPhys(const std::vector<MapData::Brush> &brushes)
    : m_brushes(brushes) {
    const glm::vec3 &center = m_brushes.GetCenter();

    GetTransform().SetPosition(center);

    m_rigidbody = g_PhysicsScene->CreateDynamic(physx::PxTransform{center.x, center.y, center.z});
    m_brushes.AttachToRigidActor(m_rigidbody);
    physx::PxRigidBodyExt::updateMassAndInertia(*m_rigidbody, 1.0f);

    m_rigidbody->userData = this;
}

AFuncPhys::~AFuncPhys() {
    PX_RELEASE(m_rigidbody)
}

void AFuncPhys::FixedUpdate(float fixedDeltaTime) {
    const physx::PxTransform transform = m_rigidbody->getGlobalPose();
    const glm::vec3          position{transform.p.x, transform.p.y, transform.p.z};
    const glm::quat          rotation{transform.q.w, transform.q.x, transform.q.y, transform.q.z};
    m_modelMatrix = GetTransform().SetPosition(position).GetTranslationMatrix() * mat4_cast(rotation);
}

void AFuncPhys::Draw() {
    m_brushes.Draw(m_modelMatrix);
}
