//
// Created by andyroiiid on 12/21/2022.
//

#include "Game.h"

#include "Globals.h"
#include "actors/ALightWorld.h"
#include "actors/APlayerNoClip.h"
#include "actors/APropTest.h"

void Game::Init(GLFWwindow *window) {
    g_Window = window;

    m_renderer = std::make_unique<Renderer>(window);
    g_Renderer = m_renderer.get();

    m_mouse = std::make_unique<Mouse>(window);
    g_Mouse = m_mouse.get();

    m_scene = std::make_unique<Scene>();
    g_Scene = m_scene.get();

    m_physicsSystem = std::make_unique<PhysicsSystem>();
    g_PhysicsSystem = m_physicsSystem.get();

    m_physicsScene = std::make_unique<PhysicsScene>(m_physicsSystem.get());
    g_PhysicsScene = m_physicsScene.get();

    m_scene->CreateActor<ALightWorld>(glm::vec3{0.75f, 0.07f, 0.65f}, glm::vec3{10.0f, 5.0f, 1.0f});
    m_scene->CreateActor<APlayerNoClip>(glm::vec3{0.0f, 0.0f, -5.0f});
    m_scene->CreateActor<APropTest>("models/boom_box.obj", "materials/boom_box.json", glm::vec3{0.0f, 0.0f, 0.0f});
    m_scene->CreateActor<APropTest>("models/damaged_helmet.obj", "materials/damaged_helmet.json", glm::vec3{2.0f, 0.0f, 0.0f});
}

void Game::Shutdown() {
    m_renderer->WaitDeviceIdle();

    g_PhysicsScene = nullptr;
    m_physicsScene.reset();

    g_PhysicsSystem = nullptr;
    m_physicsSystem.reset();

    g_Scene = nullptr;
    m_scene.reset();

    g_Mouse = nullptr;
    m_mouse.reset();

    g_Renderer = nullptr;
    m_renderer.reset();

    g_Window = nullptr;
}

void Game::Frame(float deltaTime) {
    Update(deltaTime);
    Draw();
}

void Game::Update(float deltaTime) {
    m_mouse->Update();
    m_physicsScene->Update(deltaTime, 1.0f);
    m_scene->Update(deltaTime);
}

void Game::Draw() {
    m_scene->Draw();
    m_renderer->FinishDrawing();
}
