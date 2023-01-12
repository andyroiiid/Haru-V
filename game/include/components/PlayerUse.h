//
// Created by andyroiiid on 1/11/2023.
//

#pragma once

#include <PxQueryReport.h>

class Actor;

class PlayerUse {
public:
    explicit PlayerUse(Actor *owner);

    PlayerUse(const PlayerUse &) = delete;

    PlayerUse &operator=(const PlayerUse &) = delete;

    PlayerUse(PlayerUse &&) = delete;

    PlayerUse &operator=(PlayerUse &&) = delete;

    void Update();

    [[nodiscard]] Actor *GetTarget() const { return m_prevEyeTarget; }

    void SetInput(bool pressed) { m_currButton = pressed; }

private:
    physx::PxRaycastHit EyeRayCast();

    Actor *m_owner = nullptr;

    static constexpr float INTERACTION_DISTANCE = 2.0f;

    bool m_prevButton = false;
    bool m_currButton = false;

    Actor *m_prevEyeTarget = nullptr;
};
