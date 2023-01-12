//
// Created by andyroiiid on 12/29/2022.
//

#pragma once

#include "actors/Actor.h"
#include "components/PlayerMovement.h"
#include "components/PlayerUse.h"
#include "gfx/BitmapTextRenderer.h"

class APlayer final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APlayer)

    explicit APlayer(const glm::vec3 &position, float yaw = 0.0f, float mouseSpeed = 0.001f);

    ~APlayer() override;

    void Update(float deltaTime) override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw() override;

private:
    void DrawReticle();

    PlayerMovement m_movement;
    PlayerUse      m_use;

    float m_mouseSpeed;

    bool m_prevSpace = false;
};
