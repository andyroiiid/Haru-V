//
// Created by andyroiiid on 1/4/2023.
//

#pragma once

#include "actors/Actor.h"

class ALightPoint final : public Actor {
public:
    DEFINE_ACTOR_CLASS(ALightPoint)

    explicit ALightPoint(const glm::vec3 &position, const glm::vec3 &color, float radius);

    void Draw() final;

private:
    glm::vec3 m_position;
    glm::vec3 m_color;
    float     m_radius;
};
