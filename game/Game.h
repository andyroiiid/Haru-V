//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include "window/Window.h"
#include "window/Mouse.h"
#include "math/Transform.h"

#include "gfx/Renderer.h"

class Game : public App {
public:
    void Init(GLFWwindow *window) override;

    void Shutdown() override;

    void Frame(float deltaTime) override;

private:
    void Update(float deltaTime);

    void Draw();

    GLFWwindow *m_window = nullptr;

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Mouse> m_mouse;

    VulkanMesh *m_boomBoxMesh = nullptr;
    PbrMaterial *m_boomBoxMaterial = nullptr;

    VulkanMesh *m_helmetMesh = nullptr;
    PbrMaterial *m_helmetMaterial = nullptr;

    Transform m_cameraTransform;
};
