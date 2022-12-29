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
    m_rigidbody = g_PhysicsScene->CreateStatic(physx::PxTransform{center.x, center.y, center.z});
    m_brushes.AttachToRigidActor(m_rigidbody);
    GetTransform().SetPosition(center);
}

AFuncBrush::~AFuncBrush() {
    PX_RELEASE(m_rigidbody)
}

void AFuncBrush::Draw() {
    m_brushes.Draw(GetTransform().GetMatrix());
}
