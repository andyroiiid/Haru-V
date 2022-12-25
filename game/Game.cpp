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
    AppendBoxVertices(vertices, {-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f});
    m_mesh = m_renderer->CreateMesh(vertices);

    m_cameraTransform.SetPosition({0.0f, 0.0f, -5.0f});
}

void Game::Shutdown() {
    m_renderer->WaitDeviceIdle();

    m_time = 0.0f;

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
        m_cameraTransform.Translate(glm::normalize(inputDirection) * (10.0f * deltaTime));
    }

    const glm::vec2 &deltaMousePos = m_mouse->GetDeltaPosition();
    m_cameraTransform
            .RotateX(0.001f * deltaMousePos.y)
            .RotateY(0.001f * deltaMousePos.x)
            .ClampPitch();

    m_time += deltaTime;
}

void Game::Draw() {
    const glm::mat4 projection = glm::perspective(
            glm::radians(75.0f),
            m_renderer->GetAspectRatio(),
            0.01f,
            100.0f
    );
    m_renderer->SetCameraMatrices(projection, m_cameraTransform.GetInverseMatrix());

    const glm::vec3 lightDirection{
            glm::cos(glm::radians(m_time * 90.0f)),
            1.0f,
            glm::sin(glm::radians(m_time * 90.0f))
    };
    m_renderer->SetLightingData(
            lightDirection,
            {1.0f, 1.0f, 1.0f},
            {0.2f, 0.2f, 0.2f}
    );

    const glm::mat4 IDENTITY{1.0f};

    glm::mat4 model = glm::rotate(IDENTITY, glm::radians(m_time * 30.0f), {1.0f, 1.0f, 1.0f});
    m_renderer->Draw(m_mesh, model);

    model = glm::rotate(glm::translate(IDENTITY, {2.0f, 0.0f, 0.0f}), glm::radians(m_time * 60.0f), {1.0f, 0.0f, 0.0f});
    m_renderer->Draw(m_mesh, model);

    model = glm::rotate(glm::translate(IDENTITY, {0.0f, 2.0f, 0.0f}), glm::radians(m_time * 90.0f), {0.0f, 1.0f, 0.0f});
    m_renderer->Draw(m_mesh, model);

    model = glm::rotate(glm::translate(IDENTITY, {0.0f, 0.0f, 2.0f}), glm::radians(m_time * 120.0f), {0.0f, 0.0f, 1.0f});
    m_renderer->Draw(m_mesh, model);

    m_renderer->FinishDrawing();
}
