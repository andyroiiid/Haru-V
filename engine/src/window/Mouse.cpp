//
// Created by andyroiiid on 12/24/2022.
//

#include "window/Mouse.h"

#include <GLFW/glfw3.h>

static glm::vec2 GetMousePosition(GLFWwindow *window) {
    glm::dvec2 mousePosition;
    glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
    return mousePosition;
}

Mouse::Mouse(GLFWwindow *window)
    : m_window(window)
    , m_position(GetMousePosition(window)) {
}

void Mouse::SetEnabled(bool enable) {
    glfwSetInputMode(m_window, GLFW_CURSOR, enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void Mouse::Update() {
    const glm::vec2 mousePosition = GetMousePosition(m_window);
    m_deltaPosition               = mousePosition - m_position;
    m_position                    = mousePosition;
}

bool Mouse::IsButtonDown(MouseButton button) const {
    return glfwGetMouseButton(m_window, static_cast<int>(button));
}
