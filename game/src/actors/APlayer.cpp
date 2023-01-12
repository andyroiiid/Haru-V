//
// Created by andyroiiid on 12/29/2022.
//

#include "actors/APlayer.h"

#include <GLFW/glfw3.h>
#include <PxRigidActor.h>
#include <audio/AudioSystem.h>
#include <physics/PhysicsScene.h>
#include <window/Mouse.h>

#include "Globals.h"
#include "gfx/Renderer.h"

APlayer::APlayer(const glm::vec3 &position, float yaw, float mouseSpeed)
    : m_movement(this, position) {
    GetTransform().RotateY(yaw);

    m_mouseSpeed = mouseSpeed;

    g_Mouse->SetEnabled(false);
}

APlayer::~APlayer() {
    g_Mouse->SetEnabled(true);
}

static inline float GetKeyAxis(GLFWwindow *window, int posKey, int negKey) {
    float value = 0.0f;
    if (glfwGetKey(window, posKey)) value += 1.0f;
    if (glfwGetKey(window, negKey)) value -= 1.0f;
    return value;
}

physx::PxRaycastHit APlayer::EyeRayCast() {
    const Transform &transform = GetTransform();
    const glm::vec3  position  = transform.GetPosition();
    const glm::vec3  forward   = transform.GetForwardVector();

    const physx::PxVec3 origin{position.x, position.y, position.z};
    const physx::PxVec3 unitDir{forward.x, forward.y, forward.z};
    return g_PhysicsScene->Raycast(origin, unitDir, INTERACTION_DISTANCE, PHYSICS_LAYER_0).block;
}

static Actor *GetActor(const physx::PxRaycastHit &hit) {
    physx::PxActor *actor = hit.actor;
    return actor ? static_cast<Actor *>(actor->userData) : nullptr;
}

void APlayer::UpdateInteract() {
    // eye raycast
    const physx::PxRaycastHit hit = EyeRayCast();

    // try to acquire current target
    Actor * const eyeTarget = GetActor(hit);

    // use input
    const bool currLmb = g_Mouse->IsButtonDown(MouseButton::Left);

    if (!m_prevLmb) {
        if (currLmb) {
            // lmb press
            if (eyeTarget) {
                eyeTarget->StartUse(this, hit);
            }
        }
    } else {
        if (!currLmb) {
            // lmb release
            if (m_prevEyeTarget) {
                m_prevEyeTarget->StopUse(this);
            }
        } else {
            // lmb hold
            if (eyeTarget == m_prevEyeTarget) {
                // still looking at the same object (might be null)
                if (eyeTarget) {
                    eyeTarget->ContinueUse(this, hit);
                }
            } else {
                // target changed
                if (m_prevEyeTarget) {
                    m_prevEyeTarget->StopUse(this);
                }
                if (eyeTarget) {
                    eyeTarget->StartUse(this, hit);
                }
            }
        }
    }

    m_prevLmb = currLmb;

    m_prevEyeTarget = eyeTarget;
}

void APlayer::Update(const float deltaTime) {
    m_movement.Update(deltaTime);

    Transform &transform = GetTransform();

    // turn
    {
        const float      mouseSpeed    = g_SlowMotion ? m_mouseSpeed * 0.4f : m_mouseSpeed;
        const glm::vec2 &deltaMousePos = g_Mouse->GetDeltaPosition();
        transform.RotateX(mouseSpeed * deltaMousePos.y).RotateY(mouseSpeed * deltaMousePos.x).ClampPitch();
    }

    // move
    {
        glm::vec3 movementInput = transform.GetHorizontalRightVector() * GetKeyAxis(g_Window, GLFW_KEY_D, GLFW_KEY_A) +
                                  transform.GetHorizontalForwardVector() * GetKeyAxis(g_Window, GLFW_KEY_W, GLFW_KEY_S);
        if (movementInput.x != 0 || movementInput.y != 0 || movementInput.z != 0) {
            movementInput = glm::normalize(movementInput);
        }
        m_movement.SetMovementInput(movementInput);
    }

    // jump
    {
        bool currSpace = glfwGetKey(g_Window, GLFW_KEY_SPACE);

        if (!m_prevSpace && currSpace) {
            m_movement.Jump();
        }

        m_prevSpace = currSpace;
    }

    UpdateInteract();

    g_Audio->SetListener3DAttributes(transform.GetPosition(), m_movement.GetVelocity(), transform.GetHorizontalForwardVector());
}

void APlayer::FixedUpdate(float fixedDeltaTime) {
    m_movement.FixedUpdate(fixedDeltaTime);
}

void APlayer::Draw() {
    const Transform &transform = GetTransform();
    g_Renderer->SetCameraData(transform.GetPosition(), transform.GetInverseMatrix(), glm::radians(60.0f), 0.01f, 100.0f);

    DrawReticle();
}

static void DrawReticleStandard(const glm::vec2 &position, float gap, float max, const glm::vec4 &color) {
    g_Renderer->DrawScreenLine(position + glm::vec2{gap, 0}, position + glm::vec2{max, 0}, color);
    g_Renderer->DrawScreenLine(position + glm::vec2{-gap, 0}, position + glm::vec2{-max, 0}, color);
    g_Renderer->DrawScreenLine(position + glm::vec2{0, gap}, position + glm::vec2{0, max}, color);
    g_Renderer->DrawScreenLine(position + glm::vec2{0, -gap}, position + glm::vec2{0, -max}, color);
}

static void DrawReticleDiagonal(const glm::vec2 &position, float gap, float max, const glm::vec4 &color) {
    g_Renderer->DrawScreenLine(position + glm::vec2{gap, gap}, position + glm::vec2{max, max}, color);
    g_Renderer->DrawScreenLine(position + glm::vec2{-gap, gap}, position + glm::vec2{-max, max}, color);
    g_Renderer->DrawScreenLine(position + glm::vec2{gap, -gap}, position + glm::vec2{max, -max}, color);
    g_Renderer->DrawScreenLine(position + glm::vec2{-gap, -gap}, position + glm::vec2{-max, -max}, color);
}

void APlayer::DrawReticle() {
    const glm::vec2 screenCenter = g_Renderer->GetScreenExtent() * 0.5f;

    glm::vec4 color;
    if (m_prevLmb) {
        color = {0.0f, 1.0f, 0.0f, 1.0f};
    } else {
        color = {1.0f, 1.0f, 1.0f, 1.0f};
    }

    if (m_prevEyeTarget) {
        DrawReticleDiagonal(screenCenter, 6.0f, 18.0f, color);
        m_textRenderer.DrawText(m_prevEyeTarget->GetActorClassName(), {100.0f, 100.0f});
    } else {
        DrawReticleStandard(screenCenter, 4.0f, 12.0f, color);
        m_textRenderer.DrawText("No Target", {100.0f, 100.0f});
    }
}
