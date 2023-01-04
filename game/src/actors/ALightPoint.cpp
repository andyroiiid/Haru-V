//
// Created by andyroiiid on 1/4/2023.
//
#include "actors/ALightPoint.h"

#include "Globals.h"
#include "gfx/Renderer.h"

ALightPoint::ALightPoint(const glm::vec3 &position, const glm::vec3 &color, float radius)
        : m_position(position),
          m_color(color),
          m_radius(radius) {
}

void ALightPoint::Draw() {
    g_Renderer->DrawPointLight(m_position, m_color, m_radius);
}
