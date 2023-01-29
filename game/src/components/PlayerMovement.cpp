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

    m_position = position;

    m_predictedPosition = position;
}

PlayerMovement::~PlayerMovement() {
    PX_RELEASE(m_controller)
}

void PlayerMovement::Update(float deltaTime) {
    const glm::vec3 lastEyePosition   = m_transform.GetPosition();
    const glm::vec3 predictedPosition = glm::mix(
        m_position,
        m_predictedPosition,
        g_PhysicsScene->GetFixedUpdateTimeError() / g_PhysicsScene->GetFixedTimestep()
    );
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

// based on Kalman filter from: https://github.com/Garima13a/Kalman-Filters/blob/master/2.%201D%20Kalman%20Filter%2C%20solution.ipynb

static void KalmanUpdate(glm::vec3 &posPredicted, glm::vec3 &varPredicted, const glm::vec3 &posObserved, const glm::vec3 &varObserved) {
    const glm::vec3 pos = (varObserved * posPredicted + varPredicted * posObserved) / (varPredicted + varObserved);
    const glm::vec3 var = varPredicted * varObserved / (varPredicted + varObserved);

    posPredicted = pos;
    varPredicted = var;
}

static void KalmanPredict(glm::vec3 &posPredicted, glm::vec3 &varPredicted, const glm::vec3 &deltaObserved, const glm::vec3 &varObserved) {
    const glm::vec3 mean = posPredicted + deltaObserved;
    const glm::vec3 var  = varPredicted + varObserved;

    posPredicted = mean;
    varPredicted = var;
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
    const glm::vec3     deltaPosition = currentPosition - m_position;

    m_velocity = deltaPosition / fixedDeltaTime;
    m_position = currentPosition;

    // clamp vertical speed (this is a hack)
    m_velocity.y = glm::min(m_velocity.y, JUMP_VELOCITY);

    KalmanUpdate(m_predictedPosition, m_predictedVariance, m_position, {8.0f, 8.0f, 8.0f});
    KalmanPredict(m_predictedPosition, m_predictedVariance, deltaPosition, {4.0f, 4.0f, 4.0f});
}
