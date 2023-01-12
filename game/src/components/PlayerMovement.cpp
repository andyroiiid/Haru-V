//
// Created by andyroiiid on 1/11/2023.
//
#include "components/PlayerMovement.h"

#include <PxRigidDynamic.h>
#include <characterkinematic/PxController.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>

#include "Globals.h"
#include "actors/Actor.h"

PlayerMovement::PlayerMovement(Actor *owner, const glm::vec3 &position)
    : m_transform(owner->GetTransform()) {
    m_transform.SetPosition(position);

    m_controller = g_PhysicsScene->CreateController(
        {position.x, position.y, position.z}, //
        CAPSULE_RADIUS,
        CAPSULE_HEIGHT
    );

    m_controller->getActor()->userData = owner;

    m_previousPosition = position;
}

PlayerMovement::~PlayerMovement() {
    PX_RELEASE(m_controller)
}

void PlayerMovement::Update(float deltaTime) {
    const glm::vec3     lastEyePosition = m_transform.GetPosition();
    const physx::PxVec3 position        = toVec3(m_controller->getPosition());
    const float         timeError       = glm::min(g_PhysicsScene->GetFixedTimestep(), g_PhysicsScene->GetFixedUpdateTimeError());
    const glm::vec3     predictedPosition{
        position.x + m_velocity.x * timeError,
        position.y + m_velocity.y * timeError,
        position.z + m_velocity.z * timeError};
    const glm::vec3 targetEyePosition = predictedPosition + glm::vec3{0.0f, CAPSULE_HALF_HEIGHT, 0.0f};
    m_transform.SetPosition(glm::mix(lastEyePosition, targetEyePosition, glm::min(1.0f, 30.0f * deltaTime)));
}

void PlayerMovement::CheckGround() {
    // https://nvidia-omniverse.github.io/PhysX/physx/5.1.1/docs/Geometry.html#capsules
    static const physx::PxCapsuleGeometry QUERY_GEOMETRY(CAPSULE_RADIUS, CAPSULE_HALF_HEIGHT);
    static const physx::PxQuat            QUERY_ROTATION(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f));
    static const physx::PxVec3            QUERY_DIRECTION(0.0f, -1.0f, 0.0f);

    const physx::PxSweepBuffer buffer = g_PhysicsScene->Sweep(
        QUERY_GEOMETRY,
        physx::PxTransform(toVec3(m_controller->getPosition()), QUERY_ROTATION),
        QUERY_DIRECTION,
        GROUND_CHECK_DISTANCE,
        PHYSICS_LAYER_0
    );

    // check is touching ground and ground is not too steep
    m_isOnGround = buffer.hasBlock && buffer.block.normal.y >= m_controller->getSlopeLimit();
}

void PlayerMovement::UpdateAcceleration() {
    float acceleration = m_isOnGround ? GROUND_ACCELERATION : AIR_ACCELERATION;
    float drag         = m_isOnGround ? GROUND_DRAG : AIR_DRAG;
    m_acceleration.x   = m_movementInput.x * acceleration - m_velocity.x * drag;
    m_acceleration.y   = -GRAVITY;
    m_acceleration.z   = m_movementInput.z * acceleration - m_velocity.z * drag;
}

void PlayerMovement::FixedUpdate(float fixedDeltaTime) {
    CheckGround();

    UpdateAcceleration();

    // move character controller
    m_velocity += m_acceleration * fixedDeltaTime;

    const glm::vec3     displacement = m_velocity * fixedDeltaTime;
    const physx::PxVec3 disp{displacement.x, displacement.y, displacement.z};
    m_controller->move(disp, 0.0001f, fixedDeltaTime, physx::PxControllerFilters());

    // calc projected velocity
    const physx::PxVec3 pos = toVec3(m_controller->getPosition());
    const glm::vec3     currentPosition{pos.x, pos.y, pos.z};
    m_velocity         = (currentPosition - m_previousPosition) / fixedDeltaTime;
    m_previousPosition = currentPosition;

    // clamp vertical speed (this is a hack)
    m_velocity.y = glm::min(m_velocity.y, JUMP_VELOCITY);
}
