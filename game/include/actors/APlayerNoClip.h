//
// Created by andyroiiid on 12/27/2022.
//

#pragma once

#include "actors/Actor.h"

class APlayerNoClip final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APlayerNoClip)

    explicit APlayerNoClip(const glm::vec3 &position);

    ~APlayerNoClip() final;

    void Update(float deltaTime) final;

    void Draw() final;
};
