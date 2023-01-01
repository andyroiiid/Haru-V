//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <window/Window.h>
#include <window/Mouse.h>
#include <physics/PhysicsSystem.h>
#include <physics/PhysicsScene.h>
#include <script/LuaSandbox.h>

#include "gfx/Renderer.h"
#include "actors/Scene.h"

class Game : public App {
public:
    void Init(GLFWwindow *window) override;

    void Shutdown() override;

    void Frame(float deltaTime) override;

private:
    void Update(float deltaTime);

    void Draw();

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Mouse> m_mouse;
    std::unique_ptr<PhysicsSystem> m_physicsSystem;
    std::unique_ptr<PhysicsScene> m_physicsScene;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<LuaSandbox> m_lua;
};
