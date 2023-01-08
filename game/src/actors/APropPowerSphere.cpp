//
// Created by andyroiiid on 1/4/2023.
//

#include "actors/APropPowerSphere.h"

#include <PxRigidDynamic.h>
#include <extensions/PxRigidBodyExt.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>

#include "Globals.h"
#include "actors/APlayer.h"
#include "gfx/Renderer.h"

APropPowerSphere::APropPowerSphere(const glm::vec3 &position) {
    GetTransform().SetPosition(position);
    m_position = position;

    m_rigidbody = g_PhysicsScene->CreateDynamic(
        physx::PxTransform{position.x, position.y, position.z}, //
        physx::PxSphereGeometry(0.5f)
    );
    m_rigidbody->setLinearDamping(0.1f);
    m_rigidbody->setAngularDamping(0.5f);

    m_rigidbody->userData = this;

    m_mesh     = g_Renderer->LoadObjMesh("models/power_sphere.obj");
    m_material = g_Renderer->LoadPbrMaterial("materials/power_sphere.json");
}

APropPowerSphere::~APropPowerSphere() {
    PX_RELEASE(m_rigidbody)
}

void APropPowerSphere::Update(float deltaTime) {
    const float timeError = glm::min(g_PhysicsScene->GetFixedTimestep(), g_PhysicsScene->GetFixedUpdateTimeError());

    m_translationMatrix = GetTransform().SetPosition(m_position + m_velocity * timeError).GetTranslationMatrix();
    m_rotation          = glm::slerp(m_rotation, m_currentRotation, glm::min(1.0f, 30.0f * deltaTime));

    if (m_isCharging) {
        m_chargePower += deltaTime * 5.0f;
        m_chargePower = glm::clamp(m_chargePower, 0.0f, 10.0f);
    }
}

void APropPowerSphere::FixedUpdate(float fixedDeltaTime) {
    const physx::PxTransform transform = m_rigidbody->getGlobalPose();

    const glm::vec3 lastPosition = m_position;

    m_position = {transform.p.x, transform.p.y, transform.p.z};
    m_velocity = (m_position - lastPosition) / fixedDeltaTime;

    m_currentRotation = {transform.q.w, transform.q.x, transform.q.y, transform.q.z};
}

void APropPowerSphere::Draw() {
    g_Renderer->Draw(m_mesh, m_translationMatrix * glm::mat4_cast(m_rotation), m_material);
    g_Renderer->DrawPointLight(GetTransform().GetPosition(), glm::vec3{0.5f, 0.2f, 0.0f}, 4.0f);
}

void APropPowerSphere::StartUse(APlayer *player, const physx::PxRaycastHit &hit) {
    m_isCharging = true;
}

void APropPowerSphere::ContinueUse(APlayer *player, const physx::PxRaycastHit &hit) {
    m_chargePosition = hit.position;
}

void APropPowerSphere::StopUse(APlayer *player) {
    const glm::vec3 &playerPosition = player->GetTransform().GetPosition();
    physx::PxVec3    force{
        m_chargePosition.x - playerPosition.x,
        0.0f, // no vertical force
        m_chargePosition.z - playerPosition.z,
    };
    force = force.getNormalized() * m_chargePower;
    physx::PxRigidBodyExt::addForceAtPos(*m_rigidbody, force, m_chargePosition, physx::PxForceMode::eIMPULSE);

    m_isCharging  = false;
    m_chargePower = 0.0f;
}

void APropPowerSphere::StartAltUse(APlayer *player, const physx::PxRaycastHit &hit) {
    // increase damping to stop the sphere
    m_rigidbody->setLinearDamping(5.0f);
    m_rigidbody->setAngularDamping(5.0f);
}

void APropPowerSphere::StopAltUse(APlayer *player) {
    // reset damping
    m_rigidbody->setLinearDamping(0.1f);
    m_rigidbody->setAngularDamping(0.5f);
}
