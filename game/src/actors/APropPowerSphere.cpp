//
// Created by andyroiiid on 1/4/2023.
//

#include "actors/APropPowerSphere.h"

#include <PxRigidDynamic.h>
#include <extensions/PxRigidBodyExt.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>
#include <physics/PhysicsSystem.h>
#include <physics/PhysicsUtilities.h>

#include "Globals.h"
#include "gfx/PbrRenderer.h"

APropPowerSphere::APropPowerSphere(const glm::vec3 &position) {
    GetTransform().SetPosition(position);
    m_position = position;

    m_rigidbody = g_PhysicsScene->CreateDynamic(
        physx::PxTransform{position.x, position.y, position.z}, //
        physx::PxSphereGeometry(0.5f)
    );
    EnableDamping();

    m_rigidbody->userData = this;

    m_physicsMaterial = g_PhysicsSystem->CreateMaterial(0.8f, 0.8f, 1.0f);
    PhysicsSetActorMaterial(m_rigidbody, m_physicsMaterial);

    m_mesh     = g_Renderer->LoadObjMesh("models/power_sphere.obj");
    m_material = g_Renderer->LoadPbrMaterial("materials/power_sphere.json");
}

APropPowerSphere::~APropPowerSphere() {
    PX_RELEASE(m_rigidbody)
    PX_RELEASE(m_physicsMaterial)
}

void APropPowerSphere::Update(float deltaTime) {
    const float timeError = glm::min(g_PhysicsScene->GetFixedTimestep(), g_PhysicsScene->GetFixedUpdateTimeError());

    m_translationMatrix = GetTransform().SetPosition(m_position + m_velocity * timeError).GetTranslationMatrix();
    m_rotation          = glm::slerp(m_rotation, m_currentRotation, glm::min(1.0f, 30.0f * deltaTime));
}

void APropPowerSphere::FixedUpdate(float fixedDeltaTime) {
    const physx::PxTransform transform = m_rigidbody->getGlobalPose();

    const glm::vec3 lastPosition = m_position;

    m_position = {transform.p.x, transform.p.y, transform.p.z};
    m_velocity = (m_position - lastPosition) / fixedDeltaTime;

    m_currentRotation = {transform.q.w, transform.q.x, transform.q.y, transform.q.z};

    if (m_beingPushed) {
        physx::PxRigidBodyExt::addForceAtPos(*m_rigidbody, m_pushForce, m_pushPosition, physx::PxForceMode::eFORCE);
    }
}

void APropPowerSphere::Draw() {
    g_Renderer->Draw(m_mesh, m_translationMatrix * glm::mat4_cast(m_rotation), m_material);
}

void APropPowerSphere::EnableDamping() {
    m_rigidbody->setLinearDamping(1.0f);
    m_rigidbody->setAngularDamping(1.0f);
}

void APropPowerSphere::DisableDamping() {
    m_rigidbody->setLinearDamping(0.0f);
    m_rigidbody->setAngularDamping(0.0f);
}

void APropPowerSphere::StartUse(Actor *user, const physx::PxRaycastHit &hit) {
    DisableDamping();
    m_beingPushed = true;

    ContinueUse(user, hit); // trigger the first push position/force update
}

void APropPowerSphere::ContinueUse(Actor *user, const physx::PxRaycastHit &hit) {
    const glm::vec3 &userPosition = user->GetTransform().GetPosition();

    m_pushPosition = hit.position;
    m_pushForce    = {
        m_pushPosition.x - userPosition.x,
        m_pushPosition.y - userPosition.y,
        m_pushPosition.z - userPosition.z,
    };
    m_pushForce = m_pushForce.getNormalized() * 3.0f;
}

void APropPowerSphere::StopUse(Actor *user) {
    m_beingPushed = false;
    EnableDamping();
}
