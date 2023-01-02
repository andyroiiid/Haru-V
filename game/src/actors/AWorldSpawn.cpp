//
// Created by andyroiiid on 12/31/2022.
//
#include "actors/AWorldSpawn.h"

#include <script/LuaSandbox.h>

#include "Globals.h"
#include "gfx/Renderer.h"

AWorldSpawn::AWorldSpawn(
        const std::vector<MapData::Brush> &brushes,
        const glm::vec3 &lightDirection,
        const glm::vec3 &lightColor,
        const std::string &script
) : AFuncBrush(brushes) {
    for (const auto &brush: brushes) {
        for (const auto &vertex: brush.Vertices) {
            m_geomMin.x = glm::min(vertex.x, m_geomMin.x);
            m_geomMin.y = glm::min(vertex.y, m_geomMin.y);
            m_geomMin.z = glm::min(vertex.z, m_geomMin.z);
            m_geomMax.x = glm::max(vertex.x, m_geomMax.x);
            m_geomMax.y = glm::max(vertex.y, m_geomMax.y);
            m_geomMax.z = glm::max(vertex.z, m_geomMax.z);
        }
    }

    m_lightDirection = lightDirection;
    m_lightColor = lightColor;

    if (!script.empty()) {
        g_Lua->DoFile(script);
    }
}

void AWorldSpawn::Draw() {
    g_Renderer->SetLightingData(m_lightDirection, m_lightColor);
    g_Renderer->SetWorldBounds(m_geomMin, m_geomMax);
    AFuncBrush::Draw();
}
