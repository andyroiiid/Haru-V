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
    : m_movement(this, position)
    , m_use(this) {
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

void APlayer::Update(const float deltaTime) {
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

    // use input
    m_use.SetInput(g_Mouse->IsButtonDown(MouseButton::Left));

    m_movement.Update(deltaTime);
    m_use.Update();

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
    const glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

    if (m_use.GetTarget()) {
        DrawReticleDiagonal(screenCenter, 6.0f, 18.0f, color);
    } else {
        DrawReticleStandard(screenCenter, 4.0f, 12.0f, color);
    }
}
