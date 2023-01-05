//
// Created by andyroiiid on 12/27/2022.
//

#include "actors/APlayerNoClip.h"

#include <GLFW/glfw3.h>
#include <window/Mouse.h>

#include "Globals.h"
#include "gfx/Renderer.h"

APlayerNoClip::APlayerNoClip(const glm::vec3 &position) {
    GetTransform().SetPosition(position);

    g_Mouse->SetEnabled(false);
}

APlayerNoClip::~APlayerNoClip() {
    g_Mouse->SetEnabled(true);
}

static inline float GetKeyAxis(GLFWwindow *window, int posKey, int negKey) {
    float value = 0.0f;
    if (glfwGetKey(window, posKey)) value += 1.0f;
    if (glfwGetKey(window, negKey)) value -= 1.0f;
    return value;
}

void APlayerNoClip::Update(float deltaTime) {
    const glm::vec3 inputDirection = GetTransform().GetHorizontalRightVector() * GetKeyAxis(g_Window, GLFW_KEY_D, GLFW_KEY_A) +
                                     glm::vec3(0.0f, 1.0f, 0.0f) * GetKeyAxis(g_Window, GLFW_KEY_E, GLFW_KEY_Q) +
                                     GetTransform().GetHorizontalForwardVector() * GetKeyAxis(g_Window, GLFW_KEY_W, GLFW_KEY_S);

    if (inputDirection.x != 0.0f || inputDirection.y != 0.0f || inputDirection.z != 0.0f) {
        GetTransform().Translate(glm::normalize(inputDirection) * (5.0f * deltaTime));
    }

    const glm::vec2 &deltaMousePos = g_Mouse->GetDeltaPosition();

    GetTransform().RotateX(0.001f * deltaMousePos.y).RotateY(0.001f * deltaMousePos.x).ClampPitch();
}

void APlayerNoClip::Draw() {
    g_Renderer->SetCameraData(GetTransform().GetPosition(), GetTransform().GetInverseMatrix(), glm::radians(60.0f), 0.01f, 100.0f);
}
