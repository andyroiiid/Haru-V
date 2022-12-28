//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include <glm/vec3.hpp>

#include "actors/Actor.h"

class ALightWorld final : public Actor {
public:
    DEFINE_ACTOR_CLASS(ALightWorld)

    ALightWorld(const glm::vec3 &direction, const glm::vec3 &color);

    void Draw() final;

private:
    glm::vec3 m_direction;
    glm::vec3 m_color;
};
