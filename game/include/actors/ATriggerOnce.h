//
// Created by andyroiiid on 1/9/2023.
//

#pragma once

#include "actors/ATrigger.h"

class ATriggerOnce : public ATrigger {
public:
    ATriggerOnce(const std::vector<MapData::Brush> &brushes, int targetType, std::string event)
        : ATrigger(brushes, targetType, std::move(event)) {}

    void OnTriggerEnter(Actor *other) override {
        ATrigger::OnTriggerEnter(other);
        Destroy();
    }
};
