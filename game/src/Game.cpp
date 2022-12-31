//
// Created by andyroiiid on 12/21/2022.
//

#include "Game.h"

#include <GLFW/glfw3.h>
#include <tracy/Tracy.hpp>

#include "Globals.h"
#include "actors/ALightWorld.h"
#include "actors/APropTestModel.h"
#include "map/LoadEntities.h"

void Game::Init(GLFWwindow *window) {
    ZoneScoped;

    g_Window = window;

    m_renderer = std::make_unique<Renderer>(window);
    g_Renderer = m_renderer.get();

    m_mouse = std::make_unique<Mouse>(window);
    g_Mouse = m_mouse.get();

    m_physicsSystem = std::make_unique<PhysicsSystem>();
    g_PhysicsSystem = m_physicsSystem.get();

    m_physicsScene = std::make_unique<PhysicsScene>(m_physicsSystem.get());
    g_PhysicsScene = m_physicsScene.get();

    m_scene = std::make_unique<Scene>();
    g_Scene = m_scene.get();

    LoadEntities("maps/climb.haru");

    m_scene->CreateActor<ALightWorld>(glm::vec3{0.8f, 0.35f, 0.4f}, glm::vec3{5.0f, 4.0f, 3.0f});
    m_scene->CreateActor<APropTestModel>("models/boom_box.obj", "materials/boom_box.json", glm::vec3{-2.0f, 1.0f, 0.0f});
    m_scene->CreateActor<APropTestModel>("models/damaged_helmet.obj", "materials/damaged_helmet.json", glm::vec3{2.0f, 1.0f, 0.0f});
}

void Game::Shutdown() {
    m_renderer->WaitDeviceIdle();

    g_Scene = nullptr;
    m_scene.reset();

    g_PhysicsScene = nullptr;
    m_physicsScene.reset();

    g_PhysicsSystem = nullptr;
    m_physicsSystem.reset();

    g_Mouse = nullptr;
    m_mouse.reset();

    g_Renderer = nullptr;
    m_renderer.reset();

    g_Window = nullptr;
}

void Game::Frame(float deltaTime) {
    FrameMark;

    Update(deltaTime);
    Draw();
}

void Game::Update(float deltaTime) {
    ZoneScoped;

    m_mouse->Update();

    if (glfwGetKey(g_Window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(g_Window, GLFW_TRUE);
    }

    g_SlowMotion = g_Mouse->IsButtonDown(MouseButton::Right);

    const float timeScale = g_SlowMotion ? 0.2f : 1.0f;
    if (m_physicsScene->Update(deltaTime, timeScale)) {
        m_scene->FixedUpdate(m_physicsScene->GetFixedTimestep() * timeScale);
    }
    m_scene->Update(deltaTime * timeScale);
}

void Game::Draw() {
    ZoneScoped;

    m_scene->Draw();
    m_renderer->FinishDrawing();
}
