//
// Created by andyroiiid on 12/18/2022.
//

#pragma once

#include <string>

struct GLFWwindow;

struct App {
    App() = default;

    virtual ~App() = default;

    App(const App &) = delete;

    App &operator=(const App &) = delete;

    App(App &&) = delete;

    App &operator=(App &&) = delete;

    virtual void Init(GLFWwindow *window) = 0;

    virtual void Shutdown() = 0;

    virtual void Frame(float deltaTime) = 0;
};

class Window {
public:
    Window(const std::string &name, int width, int height, bool maximized);

    ~Window();

    Window(const Window &) = delete;

    Window &operator=(const Window &) = delete;

    Window(Window &&) = delete;

    Window &operator=(Window &&) = delete;

    [[nodiscard]] GLFWwindow *const &Get() const { return m_window; }

    void MainLoop(App *app);

private:
    GLFWwindow *m_window = nullptr;
};
