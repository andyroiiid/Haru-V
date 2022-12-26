//
// Created by andyroiiid on 12/21/2022.
//

#include "Game.h"

#include <GLFW/glfw3.h>

#include "gfx/MeshUtilities.h"

void Game::Init(GLFWwindow *window) {
    m_window = window;
    m_renderer = std::make_unique<Renderer>(window);
    m_mouse = std::make_unique<Mouse>(window);

    m_mouse->SetEnabled(false);

    std::vector<VertexBase> vertices;
    AppendObjVertices(vertices, "models/boom_box.obj");
    m_mesh = m_renderer->CreateMesh(vertices);

    m_cameraTransform.SetPosition({0.0f, 0.0f, -5.0f});
    m_lightTransform.SetEulerAngles({glm::radians(-45.0f), glm::radians(150.0f), glm::radians(0.0f)});
}

void Game::Shutdown() {
    m_renderer->WaitDeviceIdle();

    m_lightTransform = {};
    m_cameraTransform = {};

    m_mesh = {};

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

    m_lightTransform
            .RotateX(GetKeyAxis(m_window, GLFW_KEY_DOWN, GLFW_KEY_UP) * deltaTime * glm::pi<float>())
            .RotateY(GetKeyAxis(m_window, GLFW_KEY_LEFT, GLFW_KEY_RIGHT) * deltaTime * glm::two_pi<float>())
            .ClampPitch();
}

void Game::Draw() {
    const glm::mat4 projection = glm::perspective(
            glm::radians(75.0f),
            m_renderer->GetAspectRatio(),
            0.01f,
            100.0f
    );
    m_renderer->SetCameraData(projection, m_cameraTransform.GetInverseMatrix(), m_cameraTransform.GetPosition());

    m_renderer->SetLightingData(
            m_lightTransform.GetForwardVector(),
            {1.0f, 1.0f, 1.0f}
    );

    const glm::mat4 IDENTITY{1.0f};
    m_renderer->Draw(m_mesh, IDENTITY);

    m_renderer->FinishDrawing();
}
