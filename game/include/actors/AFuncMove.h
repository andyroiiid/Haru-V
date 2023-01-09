//
// Created by andyroiiid on 12/29/2022.
//

#pragma once

#include <audio/AudioInstance.h>

#include "actors/AFuncBrush.h"

class AFuncMove : public AFuncBrush {
public:
    DEFINE_ACTOR_CLASS(AFuncMove)

    AFuncMove(const std::vector<MapData::Brush> &brushes, const glm::vec3 &moveSpeed, float moveTime, const std::string &moveSound = "");

    void FixedUpdate(float fixedDeltaTime) override;

    void LuaSignal(lua_State *L) override;

    [[nodiscard]] bool IsMoving() const { return m_state == State::MovingOpen || m_state == State::MovingClose; }

    [[nodiscard]] bool IsOpen() const { return m_state == State::Open; }

    [[nodiscard]] bool IsClosed() const { return m_state == State::Close; }

    void Open();

    void Close();

    void Toggle();

private:
    enum class State {
        Close,
        MovingOpen,
        Open,
        MovingClose
    };

    State m_state         = State::Close;
    float m_remainingTime = 0.0f;

    glm::vec3 m_moveSpeed{0.0f, 0.0f, 0.0f};
    float     m_moveTime = 0.0f;

    AudioInstance m_audio;
};
