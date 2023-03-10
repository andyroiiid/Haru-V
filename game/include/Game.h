//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <audio/AudioSystem.h>
#include <physics/PhysicsScene.h>
#include <physics/PhysicsSystem.h>
#include <window/Mouse.h>
#include <window/Window.h>

#include "actors/Scene.h"
#include "gfx/PbrRenderer.h"
#include "physics/PhysicsSimulationEventCallback.h"
#include "script/GameLua.h"
#include "ui/GameHUD.h"

class Game : public App {
public:
    void Init(GLFWwindow *window) override;

    void Shutdown() override;

    void Frame(float deltaTime) override;

    void ScheduleMapLoad(const std::string &mapName) { m_nextMap = mapName; }

private:
    void LoadMap(const std::string &mapName);

    void CleanupMap();

    void Update(float deltaTime);

    void Draw();

    std::string m_nextMap;
    std::string m_currentMap;

    std::unique_ptr<PbrRenderer>                    m_renderer;
    std::unique_ptr<Mouse>                          m_mouse;
    std::unique_ptr<PhysicsSystem>                  m_physicsSystem;
    std::unique_ptr<PhysicsSimulationEventCallback> m_physicsCallback;
    std::unique_ptr<AudioSystem>                    m_audio;

    // recreated per map
    std::unique_ptr<PhysicsScene> m_physicsScene;
    std::unique_ptr<Scene>        m_scene;
    std::unique_ptr<GameLua>      m_lua;
    std::unique_ptr<GameHUD>      m_hud;

    bool m_prevR = false;
};
