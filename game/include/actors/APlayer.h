//
// Created by andyroiiid on 12/29/2022.
//

#pragma once

#include "actors/Actor.h"

namespace physx {
    class PxController;
}

class APlayer final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APlayer)

    explicit APlayer(
            const glm::vec3 &position,
            float yaw = 0.0f,
            float mouseSpeed = 0.001f
    );

    ~APlayer() override;

    void Update(float deltaTime) override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw() override;

private:
    void UpdateGround();

    void CalcHorizontalAcceleration(const glm::vec3 &direction, float acceleration, float drag);

    void UpdateAcceleration();

    static constexpr float CAPSULE_RADIUS = 0.4f;
    static constexpr float CAPSULE_HALF_HEIGHT = 0.4f;
    static constexpr float CAPSULE_HEIGHT = CAPSULE_HALF_HEIGHT * 2.0f;

    static constexpr float INTERACTION_DISTANCE = 2.0f;
    static constexpr float GROUND_CHECK_DISTANCE = 0.3f;

    static constexpr float GRAVITY = 20.0f;
    static constexpr float GROUND_ACCELERATION = 50.0f;
    static constexpr float GROUND_DRAG = 10.0f;
    static constexpr float AIR_ACCELERATION = 10.0f;
    static constexpr float AIR_DRAG = 2.0f;
    static constexpr float JUMP_VELOCITY = 7.0f;

    physx::PxController *m_controller;

    float m_mouseSpeed;
    glm::vec3 m_movementInput{};

    bool m_isOnGround = false;

    glm::vec3 m_velocity{};
    glm::vec3 m_acceleration{};

    glm::vec3 m_respawnPoint{};
    glm::vec3 m_previousPosition{};

    bool m_prevSpace = false;

    bool m_prevLmb = false;
    Actor *m_hitActor = nullptr;
};
