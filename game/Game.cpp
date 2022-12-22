//
// Created by andyroiiid on 12/21/2022.
//

#include "Game.h"

#include <glm/gtc/matrix_transform.hpp>

#include "MeshUtilities.h"

void Game::Init(GLFWwindow *window) {
    m_renderer = std::make_unique<Renderer>(window);

    std::vector<VertexBase> vertices;
    AppendBoxVertices(vertices, {-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f});
    m_mesh = m_renderer->CreateMesh(vertices);
}

void Game::Shutdown() {
    m_renderer->WaitDeviceIdle();

    m_mesh = {};
    m_renderer.reset();
}

void Game::Frame(float deltaTime) {
    m_time += deltaTime;

    const glm::mat4 projection = glm::perspective(
            glm::radians(75.0f),
            m_renderer->GetAspectRatio(),
            0.01f,
            100.0f
    );
    const glm::mat4 view = glm::lookAt(
            glm::vec3{glm::cos(m_time) * 5.0f, 4.0f, glm::sin(m_time) * 5.0f},
            glm::vec3{0.0f, 0.0f, 0.0f},
            glm::vec3{0.0f, 1.0f, 0.0f}
    );

    m_renderer->SetCameraMatrices(projection, view);

    const glm::mat4 IDENTITY{1.0f};

    glm::mat4 model = glm::rotate(IDENTITY, glm::radians(m_time * 30.0f), {1.0f, 1.0f, 1.0f});
    m_renderer->Draw(m_mesh, model);

    model = glm::rotate(glm::translate(IDENTITY, {2.0f, 0.0f, 0.0f}), glm::radians(m_time * 60.0f), {1.0f, 0.0f, 0.0f});
    m_renderer->Draw(m_mesh, model);

    model = glm::rotate(glm::translate(IDENTITY, {0.0f, 2.0f, 0.0f}), glm::radians(m_time * 90.0f), {0.0f, 1.0f, 0.0f});
    m_renderer->Draw(m_mesh, model);

    model = glm::rotate(glm::translate(IDENTITY, {0.0f, 0.0f, 2.0f}), glm::radians(m_time * 120.0f), {0.0f, 0.0f, 1.0f});
    m_renderer->Draw(m_mesh, model);

    m_renderer->DrawToScreen();
}
