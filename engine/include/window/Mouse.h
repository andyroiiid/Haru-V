//
// Created by andyroiiid on 12/24/2022.
//

#pragma once

#include <glm/vec2.hpp>

struct GLFWwindow;

enum class MouseButton : int {
    Left   = 0,
    Right  = 1,
    Middle = 2
};

class Mouse {
public:
    explicit Mouse(GLFWwindow *window);

    ~Mouse() = default;

    Mouse(const Mouse &) = delete;

    Mouse &operator=(const Mouse &) = delete;

    Mouse(Mouse &&) = delete;

    Mouse &operator=(Mouse &&) = delete;

    void Recalibrate();

    void SetEnabled(bool enable);

    void Update();

    [[nodiscard]] bool IsButtonDown(MouseButton button) const;

    [[nodiscard]] const glm::vec2 &GetPosition() const { return m_position; }

    [[nodiscard]] const glm::vec2 &GetDeltaPosition() const { return m_deltaPosition; }

private:
    GLFWwindow *m_window = nullptr;

    glm::vec2 m_position{};
    glm::vec2 m_deltaPosition{};
};
