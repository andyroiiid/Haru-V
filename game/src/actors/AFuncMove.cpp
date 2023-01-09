//
// Created by andyroiiid on 12/29/2022.
//

#include "actors/AFuncMove.h"

#include <audio/AudioSystem.h>
#include <lua.hpp>

#include "Globals.h"

AFuncMove::AFuncMove(const std::vector<MapData::Brush> &brushes, const glm::vec3 &moveSpeed, float moveTime, const std::string &moveSound)
    : AFuncBrush(brushes)
    , m_moveSpeed(moveSpeed)
    , m_moveTime(moveTime) {
    if (!moveSound.empty()) {
        m_audio = g_Audio->CreateInstance(moveSound);
    }
}

void AFuncMove::FixedUpdate(float fixedDeltaTime) {
    AFuncBrush::FixedUpdate(fixedDeltaTime);

    switch (m_state) {
    case State::MovingOpen: {
        float moveTime = glm::min(m_remainingTime, fixedDeltaTime);
        Move(m_moveSpeed * moveTime);
        m_remainingTime -= moveTime;
        if (m_remainingTime <= 0.0f) {
            m_audio.Stop();
            m_state = State::Open;
        }
        break;
    }
    case State::MovingClose: {
        float moveTime = glm::min(m_remainingTime, fixedDeltaTime);
        Move(-m_moveSpeed * moveTime);
        m_remainingTime -= moveTime;
        if (m_remainingTime <= 0.0f) {
            m_audio.Stop();
            m_state = State::Close;
        }
        break;
    }
    default:
        break;
    }

    m_audio.Set3DAttributes(GetTransform().GetPosition());
}

void AFuncMove::LuaSignal(lua_State *L) {
    const std::string event = luaL_checkstring(L, 2);
    if (event == "open") {
        Open();
    } else if (event == "close") {
        Close();
    } else if (event == "toggle") {
        Toggle();
    } else {
        luaL_error(L, "func_move doesn't recognize event %s", event.c_str());
    }
}

void AFuncMove::Open() {
    switch (m_state) {
    case State::Close:
        m_audio.Start();
        m_state         = State::MovingOpen;
        m_remainingTime = m_moveTime;
        break;
    case State::MovingClose:
        m_state         = State::MovingOpen;
        m_remainingTime = m_moveTime - m_remainingTime;
        break;
    default:
        break;
    }
}

void AFuncMove::Close() {
    switch (m_state) {
    case State::Open:
        m_audio.Start();
        m_state         = State::MovingClose;
        m_remainingTime = m_moveTime;
        break;
    case State::MovingOpen:
        m_state         = State::MovingClose;
        m_remainingTime = m_moveTime - m_remainingTime;
        break;
    default:
        break;
    }
}

void AFuncMove::Toggle() {
    switch (m_state) {
    case State::Close:
    case State::MovingClose:
        Open();
        break;
    case State::Open:
    case State::MovingOpen:
        Close();
        break;
    }
}
