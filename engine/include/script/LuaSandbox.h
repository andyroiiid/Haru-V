//
// Created by andyroiiid on 1/1/2023.
//

#pragma once

#include <lua.hpp>
#include <string>

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

    void SetGlobalFunction(const std::string &name, lua_CFunction function);

    void CallGlobalFunction(const std::string &name);

    void DoFile(const std::string &filename);

private:
    void PCall();

    lua_State *L = nullptr;
};
