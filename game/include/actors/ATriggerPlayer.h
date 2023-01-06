//
// Created by andyroiiid on 1/6/2023.
//

#pragma once

#include "actors/AFuncBrush.h"

class ATriggerPlayer : public AFuncBrush {
public:
    DEFINE_ACTOR_CLASS(ATriggerPlayer)

    ATriggerPlayer(const std::vector<MapData::Brush> &brushes, std::string event)
        : AFuncBrush(brushes, BrushType::Trigger, PHYSICS_LAYER_1)
        , m_event(std::move(event)) {}

    void OnTriggerEnter(Actor *other) override;

    void OnTriggerExit(Actor *other) override;

private:
    std::string m_event;
};
