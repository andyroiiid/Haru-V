//
// Created by andyroiiid on 12/18/2022.
//

#include "window/Window.h"

#include <GLFW/glfw3.h>

#include "core/Debug.h"

Window::Window(const std::string &name, int width, int height, bool maximized) {
    glfwSetErrorCallback([](int errorCode, const char *description) {
        DebugError("GLFW error {}: {}", errorCode, description);
    });

    DebugCheckCritical(glfwInit() == GLFW_TRUE, "Failed to init GLFW.");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_MAXIMIZED, maximized ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    DebugCheckCritical(m_window != nullptr, "Failed to create GLFW window.");

    DebugInfo("GLFW {}.{}.{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::MainLoop(App *app) {
    app->Init(m_window);

    glfwShowWindow(m_window);

    double prevTime = glfwGetTime();
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        const double currTime = glfwGetTime();
        app->Frame(static_cast<float>(currTime - prevTime));
        prevTime = currTime;
    }

    app->Shutdown();
}
