//
// Created by andyroiiid on 12/21/2022.
//

#include "Game.h"

#include <glm/gtc/matrix_transform.hpp>

#include "MeshUtilities.h"

void Game::Init(GLFWwindow *window) {
    m_renderer = std::make_unique<Renderer>(window);

    std::vector<VertexBase> vertices;
    AppendBoxVertices(vertices, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
    m_mesh = m_renderer->CreateMesh(vertices);
}

void Game::Shutdown() {
    m_renderer->WaitDeviceIdle();

    m_mesh = {};
    m_renderer.reset();
}

void Game::Frame(float deltaTime) {
    const glm::mat4 projection = glm::perspective(
            glm::radians(75.0f),
            m_renderer->GetAspectRatio(),
            0.01f,
            100.0f
    );
    const glm::mat4 view = glm::lookAt(
            glm::vec3{3.0f, 4.0f, -5.0f},
            glm::vec3{0.0f, 0.0f, 0.0f},
            glm::vec3{0.0f, 1.0f, 0.0f}
    );

    const glm::mat4 IDENTITY{1.0f};

    glm::mat4 model = IDENTITY;
    glm::mat4 matrix = projection * view * model;
    m_renderer->Draw(m_mesh, matrix);

    model = glm::translate(IDENTITY, {2.0f, 0.0f, 0.0f});
    matrix = projection * view * model;
    m_renderer->Draw(m_mesh, matrix);

    model = glm::translate(IDENTITY, {0.0f, 2.0f, 0.0f});
    matrix = projection * view * model;
    m_renderer->Draw(m_mesh, matrix);

    model = glm::translate(IDENTITY, {0.0f, 0.0f, 2.0f});
    matrix = projection * view * model;
    m_renderer->Draw(m_mesh, matrix);

    m_renderer->DrawToScreen();
}
