//
// Created by andyroiiid on 12/29/2022.
//

#include "actors/APlayer.h"

#include <GLFW/glfw3.h>
#include <PxRigidDynamic.h>
#include <characterkinematic/PxController.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>
#include <window/Mouse.h>

#include "Globals.h"
#include "gfx/Renderer.h"

APlayer::APlayer(const glm::vec3 &position, float yaw, float mouseSpeed) {
    GetTransform().SetPosition(position).RotateY(yaw);

    m_controller = g_PhysicsScene->CreateController({position.x, position.y, position.z}, CAPSULE_RADIUS, CAPSULE_HEIGHT);

    m_controller->getActor()->userData = this;

    m_mouseSpeed = mouseSpeed;

    m_respawnPoint     = position;
    m_previousPosition = position;

    g_Mouse->SetEnabled(false);
}

APlayer::~APlayer() {
    g_Mouse->SetEnabled(true);

    PX_RELEASE(m_controller)
}

static inline float GetKeyAxis(GLFWwindow *window, int posKey, int negKey) {
    float value = 0.0f;
    if (glfwGetKey(window, posKey)) value += 1.0f;
    if (glfwGetKey(window, negKey)) value -= 1.0f;
    return value;
}

void APlayer::Update(const float deltaTime) {
    Transform &transform = GetTransform();

    // sync position
    {
        const physx::PxVec3 position  = toVec3(m_controller->getPosition());
        const float         timeError = glm::min(g_PhysicsScene->GetFixedTimestep(), g_PhysicsScene->GetFixedUpdateTimeError());
        const glm::vec3     predictedPosition{
            position.x + m_velocity.x * timeError,
            position.y + m_velocity.y * timeError,
            position.z + m_velocity.z * timeError};
        transform.SetPosition(predictedPosition + glm::vec3{0.0f, CAPSULE_HALF_HEIGHT, 0.0f});
    }

    // turn
    {
        const float      mouseSpeed    = g_SlowMotion ? m_mouseSpeed * 0.4f : m_mouseSpeed;
        const glm::vec2 &deltaMousePos = g_Mouse->GetDeltaPosition();
        transform.RotateX(mouseSpeed * deltaMousePos.y).RotateY(mouseSpeed * deltaMousePos.x).ClampPitch();
    }

    // move
    {
        m_movementInput = transform.GetHorizontalRightVector() * GetKeyAxis(g_Window, GLFW_KEY_D, GLFW_KEY_A) +
                          transform.GetHorizontalForwardVector() * GetKeyAxis(g_Window, GLFW_KEY_W, GLFW_KEY_S);
    }

    // jump
    {
        bool currSpace = glfwGetKey(g_Window, GLFW_KEY_SPACE);

        if (!m_prevSpace && currSpace && m_isOnGround) {
            m_velocity.y = JUMP_VELOCITY;
        }

        m_prevSpace = currSpace;
    }

    // raycast for current target
    {
        bool currLmb = g_Mouse->IsButtonDown(MouseButton::Left);

        const glm::vec3 position = transform.GetPosition();
        const glm::vec3 forward  = transform.GetForwardVector();

        const physx::PxVec3    origin{position.x, position.y, position.z};
        const physx::PxVec3    unitDir{forward.x, forward.y, forward.z};
        physx::PxRaycastBuffer buffer = g_PhysicsScene->Raycast(origin, unitDir, INTERACTION_DISTANCE, PHYSICS_LAYER_0);

        m_hitActor = nullptr;
        if (buffer.hasBlock) {
            const physx::PxRaycastHit &hit = buffer.block;

            if (hit.actor) {
                m_hitActor = static_cast<Actor *>(hit.actor->userData);

                if (!m_prevLmb && currLmb && m_hitActor) {
                    m_hitActor->Use(this, hit);
                }
            }
        }

        m_prevLmb = currLmb;
    }
}

void APlayer::UpdateGround() {
    // https://nvidia-omniverse.github.io/PhysX/physx/5.1.1/docs/Geometry.html#capsules
    static const physx::PxCapsuleGeometry QUERY_GEOMETRY(CAPSULE_RADIUS, CAPSULE_HALF_HEIGHT);
    static const physx::PxQuat            QUERY_ROTATION(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f));
    static const physx::PxVec3            QUERY_DIRECTION(0.0f, -1.0f, 0.0f);

    physx::PxSweepBuffer buffer = g_PhysicsScene->Sweep(
        QUERY_GEOMETRY,
        physx::PxTransform(toVec3(m_controller->getPosition()), QUERY_ROTATION),
        QUERY_DIRECTION,
        GROUND_CHECK_DISTANCE,
        PHYSICS_LAYER_0
    );

    // check is touching ground and ground is not too steep
    m_isOnGround = buffer.hasBlock && buffer.block.normal.y >= m_controller->getSlopeLimit();
}

void APlayer::CalcHorizontalAcceleration(const glm::vec3 &direction, float acceleration, float drag) {
    m_acceleration.x = direction.x * acceleration - m_velocity.x * drag;
    m_acceleration.z = direction.z * acceleration - m_velocity.z * drag;
}

void APlayer::UpdateAcceleration() {
    m_acceleration     = {};
    float acceleration = m_isOnGround ? GROUND_ACCELERATION : AIR_ACCELERATION;
    float drag         = m_isOnGround ? GROUND_DRAG : AIR_DRAG;
    CalcHorizontalAcceleration(m_movementInput, acceleration, drag);
    m_acceleration.y = -GRAVITY;
}

void APlayer::FixedUpdate(float fixedDeltaTime) {
    UpdateGround();

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

    // check respawn
    if (currentPosition.y < -100.0f) {
        // just teleport player back
        m_controller->setPosition({m_respawnPoint.x, m_respawnPoint.y, m_respawnPoint.z});
        m_velocity         = {};
        m_acceleration     = {};
        m_previousPosition = m_respawnPoint;
    }
}

void APlayer::Draw() {
    const Transform &transform = GetTransform();

    g_Renderer->SetCameraData(transform.GetPosition(), transform.GetInverseMatrix(), glm::radians(60.0f), 0.01f, 100.0f);
}
