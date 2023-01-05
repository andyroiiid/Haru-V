//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <audio/AudioSystem.h>
#include <physics/PhysicsScene.h>
#include <physics/PhysicsSystem.h>
#include <script/LuaSandbox.h>
#include <window/Mouse.h>
#include <window/Window.h>

#include "actors/Scene.h"
#include "gfx/Renderer.h"
#include "physics/PhysicsSimulationEventCallback.h"

class Game : public App {
public:
    explicit Game(std::string startMap)
        : m_startMap(std::move(startMap)) {}

    void Init(GLFWwindow *window) override;

    void Shutdown() override;

    void Frame(float deltaTime) override;

private:
    void Update(float deltaTime);

    void Draw();

    std::string m_startMap;

    std::unique_ptr<Renderer>                       m_renderer;
    std::unique_ptr<Mouse>                          m_mouse;
    std::unique_ptr<PhysicsSystem>                  m_physicsSystem;
    std::unique_ptr<PhysicsScene>                   m_physicsScene;
    std::unique_ptr<PhysicsSimulationEventCallback> m_physicsCallback;
    std::unique_ptr<Scene>                          m_scene;
    std::unique_ptr<AudioSystem>                    m_audio;
    std::unique_ptr<LuaSandbox>                     m_lua;
};
