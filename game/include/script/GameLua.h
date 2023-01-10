//
// Created by andyroiiid on 1/10/2023.
//

#pragma once

#include <script/LuaSandbox.h>
#include <vector>

class GameLua : public LuaSandbox {
public:
    GameLua();

    GameLua(const GameLua &) = delete;

    GameLua &operator=(const GameLua &) = delete;

    GameLua(GameLua &&) = delete;

    GameLua &operator=(GameLua &&) = delete;

    void Update(float deltaTime);

private:
    std::vector<std::tuple<float, int>> m_delayedLuaFunctions;
    std::vector<std::tuple<float, int>> m_queuedDelayedLuaFunctions;
};
