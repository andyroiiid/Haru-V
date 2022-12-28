//
// Created by andyroiiid on 12/21/2022.
//

#include "Game.h"

#include <GLFW/glfw3.h>

#include "actors/APropTest.h"

void Game::Init(GLFWwindow *window) {
    m_window = window;
    m_renderer = std::make_unique<Renderer>(window);
    m_mouse = std::make_unique<Mouse>(window);
    m_scene = std::make_unique<Scene>();

    m_mouse->SetEnabled(false);

    m_scene->CreateActor<APropTest>(m_renderer.get(), "models/boom_box.obj", "materials/boom_box.json", glm::vec3{0.0f, 0.0f, 0.0f});
    m_scene->CreateActor<APropTest>(m_renderer.get(), "models/damaged_helmet.obj", "materials/damaged_helmet.json", glm::vec3{2.0f, 0.0f, 0.0f});

    m_cameraTransform.SetPosition({0.0f, 0.0f, -5.0f});
}

void Game::Shutdown() {
    m_renderer->WaitDeviceIdle();

    m_cameraTransform = {};

    m_scene.reset();
    m_mouse.reset();
    m_renderer.reset();
    m_window = nullptr;
}

void Game::Frame(float deltaTime) {
    Update(deltaTime);
    Draw();
}

static inline float GetKeyAxis(GLFWwindow *window, int posKey, int negKey) {
    float value = 0.0f;
    if (glfwGetKey(window, posKey)) value += 1.0f;
    if (glfwGetKey(window, negKey)) value -= 1.0f;
    return value;
}

void Game::Update(float deltaTime) {
    m_mouse->Update();

    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }

    const glm::vec3 inputDirection =
            m_cameraTransform.GetHorizontalRightVector() * GetKeyAxis(m_window, GLFW_KEY_D, GLFW_KEY_A) +
            glm::vec3(0.0f, 1.0f, 0.0f) * GetKeyAxis(m_window, GLFW_KEY_E, GLFW_KEY_Q) +
            m_cameraTransform.GetHorizontalForwardVector() * GetKeyAxis(m_window, GLFW_KEY_W, GLFW_KEY_S);
    if (inputDirection.x != 0.0f || inputDirection.y != 0.0f || inputDirection.z != 0.0f) {
        m_cameraTransform.Translate(glm::normalize(inputDirection) * (5.0f * deltaTime));
    }

    const glm::vec2 &deltaMousePos = m_mouse->GetDeltaPosition();
    m_cameraTransform
            .RotateX(0.001f * deltaMousePos.y)
            .RotateY(0.001f * deltaMousePos.x)
            .ClampPitch();

    m_scene->Update(deltaTime);
}

void Game::Draw() {
    const glm::mat4 projection = glm::perspective(
            glm::radians(60.0f),
            m_renderer->GetAspectRatio(),
            0.01f,
            100.0f
    );
    m_renderer->SetCameraData(projection, m_cameraTransform.GetInverseMatrix(), m_cameraTransform.GetPosition());

    m_renderer->SetLightingData(
            {0.75f, 0.07f, 0.65f},
            {10.0f, 5.0f, 1.0f}
    );

    m_scene->Draw(m_renderer.get());

    m_renderer->FinishDrawing();
}
