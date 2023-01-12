//
// Created by andyroiiid on 1/11/2023.
//

#pragma once

#include <math/Transform.h>

namespace physx {
class PxController;
} // namespace physx

class Actor;

class PlayerMovement {
public:
    PlayerMovement(Actor *owner, const glm::vec3 &position);

    ~PlayerMovement();

    PlayerMovement(const PlayerMovement &) = delete;

    PlayerMovement &operator=(const PlayerMovement &) = delete;

    PlayerMovement(PlayerMovement &&) = delete;

    PlayerMovement &operator=(PlayerMovement &&) = delete;

    void Update(float deltaTime);

    void FixedUpdate(float fixedDeltaTime);

    [[nodiscard]] bool CanJump() const { return m_isOnGround; }

    [[nodiscard]] const glm::vec3 &GetVelocity() const { return m_velocity; }

    void SetMovementInput(const glm::vec3 &movementInput) { m_movementInput = movementInput; }

    void Launch(float yVelocity) { m_velocity.y = yVelocity; }

    void Jump() {
        if (CanJump()) {
            Launch(JUMP_VELOCITY);
        }
    }

private:
    void CheckGround();

    void UpdateAcceleration();

    static constexpr float CAPSULE_RADIUS      = 0.4f;
    static constexpr float CAPSULE_HALF_HEIGHT = 0.4f;
    static constexpr float CAPSULE_HEIGHT      = CAPSULE_HALF_HEIGHT * 2.0f;

    static constexpr float GROUND_CHECK_DISTANCE = 0.3f;

    static constexpr float GRAVITY             = 20.0f;
    static constexpr float GROUND_ACCELERATION = 50.0f;
    static constexpr float GROUND_DRAG         = 10.0f;
    static constexpr float AIR_ACCELERATION    = 10.0f;
    static constexpr float AIR_DRAG            = 2.0f;
    static constexpr float JUMP_VELOCITY       = 7.0f;

    Transform &m_transform;

    physx::PxController *m_controller = nullptr;

    glm::vec3 m_movementInput{};

    bool m_isOnGround = false;

    glm::vec3 m_velocity{};
    glm::vec3 m_acceleration{};

    glm::vec3 m_previousPosition{};
};
