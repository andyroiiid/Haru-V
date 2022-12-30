//
// Created by andyroiiid on 12/29/2022.
//

#include "actors/AFuncMove.h"

void AFuncMove::FixedUpdate(float fixedDeltaTime) {
    AFuncBrush::FixedUpdate(fixedDeltaTime);

    switch (m_state) {
        case State::MovingOpen: {
            float moveTime = glm::min(m_remainingTime, fixedDeltaTime);
            Move(m_moveSpeed * moveTime);
            m_remainingTime -= moveTime;
            if (m_remainingTime <= 0.0f) {
                m_state = State::Open;
            }
            break;
        }
        case State::MovingClose: {
            float moveTime = glm::min(m_remainingTime, fixedDeltaTime);
            Move(-m_moveSpeed * moveTime);
            m_remainingTime -= moveTime;
            if (m_remainingTime <= 0.0f) {
                m_state = State::Close;
            }
            break;
        }
        default:
            break;
    }
}

void AFuncMove::Use(APlayer *player, const physx::PxRaycastHit &hit) {
    AFuncBrush::Use(player, hit);

    Toggle();
}

void AFuncMove::Open() {
    switch (m_state) {
        case State::Close:
            m_state = State::MovingOpen;
            m_remainingTime = m_moveTime;
            break;
        case State::MovingClose:
            m_state = State::MovingOpen;
            m_remainingTime = m_moveTime - m_remainingTime;
            break;
        default:
            break;
    }
}

void AFuncMove::Close() {
    switch (m_state) {
        case State::Open:
            m_state = State::MovingClose;
            m_remainingTime = m_moveTime;
            break;
        case State::MovingOpen:
            m_state = State::MovingClose;
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
