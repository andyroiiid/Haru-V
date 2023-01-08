//
// Created by andyroiiid on 1/1/2023.
//

#pragma once

#include "actors/AFuncMove.h"

class AFuncButton : public AFuncMove {
public:
    DEFINE_ACTOR_CLASS(AFuncButton)

    AFuncButton(const std::vector<MapData::Brush> &brushes, const glm::vec3 &moveSpeed, float moveTime, std::string event)
        : AFuncMove(brushes, moveSpeed, moveTime)
        , m_event(std::move(event)) {}

    void StartUse(APlayer *player, const physx::PxRaycastHit &hit) override;

    void StopUse(APlayer *player) override;

private:
    std::string m_event;
};
