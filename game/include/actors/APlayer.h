//
// Created by andyroiiid on 12/29/2022.
//

#pragma once

#include "actors/Actor.h"
#include "components/PlayerMovement.h"
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
    physx::PxRaycastHit EyeRayCast();

    void UpdateInteract();

    void DrawReticle();

    PlayerMovement m_movement;

    BitmapTextRenderer m_textRenderer{
        "textures/bitmap_fonts/share_tech_mono.png",
        {23.0f, 48.0f}
    };

    static constexpr float INTERACTION_DISTANCE = 2.0f;

    float m_mouseSpeed;

    bool m_prevSpace = false;

    bool m_prevLmb = false;

    Actor *m_prevEyeTarget = nullptr;
};
