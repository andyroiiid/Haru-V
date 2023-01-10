//
// Created by andyroiiid on 1/6/2023.
//

#pragma once

#include "actors/AFuncBrush.h"

class ATrigger : public AFuncBrush {
public:
    DEFINE_ACTOR_CLASS(ATrigger)

    enum TargetType {
        TARGET_TYPE_PLAYER       = 0,
        TARGET_TYPE_POWER_SPHERE = 1,
    };

    ATrigger(const std::vector<MapData::Brush> &brushes, int targetType, std::string event)
        : AFuncBrush(brushes, BrushType::Trigger, PHYSICS_LAYER_1)
        , m_targetType(static_cast<TargetType>(targetType))
        , m_event(std::move(event)) {}

    void Draw() override;

    void OnTriggerEnter(Actor *other) override;

    void OnTriggerExit(Actor *other) override;

private:
    TargetType  m_targetType;
    std::string m_event;
};
