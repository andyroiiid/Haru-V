//
// Created by andyroiiid on 1/1/2023.
//

#pragma once

#include <string>
#include <lua.hpp>

class LuaSandbox {
public:
    LuaSandbox();

    ~LuaSandbox();

    LuaSandbox(const LuaSandbox &) = delete;

    LuaSandbox &operator=(const LuaSandbox &) = delete;

    LuaSandbox(LuaSandbox &&) = delete;

    LuaSandbox &operator=(LuaSandbox &&) = delete;

    void DebugStackItem(int idx);

    void DebugStack();

    void SetFunction(const std::string &name, lua_CFunction function);

    void DoFile(const std::string &filename);

private:
    lua_State *L = nullptr;
};
