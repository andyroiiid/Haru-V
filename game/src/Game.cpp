//
// Created by andyroiiid on 12/21/2022.
//

#include "Game.h"

#include <GLFW/glfw3.h>
#include <tracy/Tracy.hpp>

#include "Globals.h"
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

    m_physicsCallback = std::make_unique<PhysicsSimulationEventCallback>();
    m_physicsScene->SetSimulationEventCallback(m_physicsCallback.get());

    m_scene = std::make_unique<Scene>();
    g_Scene = m_scene.get();

    m_audio = std::make_unique<AudioSystem>();
    g_Audio = m_audio.get();

    m_lua = std::make_unique<LuaSandbox>();
    g_Lua = m_lua.get();

    m_lua->SetGlobalFunction("signal", [](lua_State *L) {
        const std::string name  = luaL_checkstring(L, 1);
        Actor            *actor = g_Scene->FindActorWithName(name);
        if (actor != nullptr) {
            actor->LuaSignal(L);
        }
        return 0;
    });

    m_lua->SetGlobalFunction("load_audio_bank", [](lua_State *L) {
        const std::string bank = luaL_checkstring(L, 1);
        g_Audio->LoadBank(bank);
        return 0;
    });

    m_lua->SetGlobalFunction("play_audio_one_shot", [](lua_State *L) {
        const std::string path = luaL_checkstring(L, 1);
        g_Audio->PlayOneShot(path);
        return 0;
    });

    LoadEntities(m_startMap);
}

void Game::Shutdown() {
    m_renderer->WaitDeviceIdle();

    g_Lua = nullptr;
    m_lua.reset();

    g_Audio = nullptr;
    m_audio.reset();

    g_Scene = nullptr;
    m_scene.reset();

    m_physicsCallback.reset();

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
    m_audio->Update();

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
