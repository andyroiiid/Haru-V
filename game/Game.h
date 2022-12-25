//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include "window/Window.h"

#include "gfx/Renderer.h"

class Game : public App {
public:
    void Init(GLFWwindow *window) override;

    void Shutdown() override;

    void Frame(float deltaTime) override;

private:
    std::unique_ptr<Renderer> m_renderer;
    VulkanMesh m_mesh;

    float m_time = 0.0f;
};
