//
// Created by andyroiiid on 12/28/2022.
//

#include "actors/ALightWorld.h"

#include "Globals.h"
#include "gfx/Renderer.h"

ALightWorld::ALightWorld(const glm::vec3 &direction, const glm::vec3 &color)
        : m_direction(direction),
          m_color(color) {
}

void ALightWorld::Draw() {
    g_Renderer->SetLightingData(m_direction, m_color);
}
