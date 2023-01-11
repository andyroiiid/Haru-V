//
// Created by andyroiiid on 1/10/2023.
//
#include "ui/GameHUD.h"

#include "Globals.h"
#include "gfx/Renderer.h"

GameHUD::GameHUD() {
    m_whiteTexture = g_Renderer->LoadSingleTextureMaterial("textures/dev/white.png");
}

void GameHUD::Update(float deltaTime) {
    if (m_fadeTimer > 0.0f) {
        m_fadeTimer = glm::max(0.0f, m_fadeTimer - deltaTime);
    }
}

void GameHUD::Draw() {
    const glm::vec2 screenExtent = g_Renderer->GetScreenExtent();

    if (m_fadeTimer > 0.0f) {
        const float fade = m_fadeTimer / m_fadeDuration;
        g_Renderer->DrawScreenRect(
            {0.0f, 0.0f}, //
            screenExtent,
            {0.0f, 0.0f},
            {1.0f, 1.0f},
            {m_fadeColor.x, //
             m_fadeColor.y,
             m_fadeColor.z,
             m_fadeTo ? 1.0f - fade : fade},
            m_whiteTexture
        );
    }
}
