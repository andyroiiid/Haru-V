//
// Created by andyroiiid on 1/10/2023.
//
#include "script/GameLua.h"

#include "Game.h"
#include "Globals.h"

GameLua::GameLua() {
    SetGlobalFunction("loadMap", [](lua_State *L) {
        const std::string mapName = luaL_checkstring(L, 1);
        g_Game->ScheduleMapLoad(mapName);
        return 0;
    });

    SetGlobalFunction("signal", [](lua_State *L) {
        const std::string name  = luaL_checkstring(L, 1);
        Actor            *actor = g_Scene->FindActorWithName(name);
        if (actor != nullptr) {
            actor->LuaSignal(L);
        }
        return 0;
    });

    SetGlobalFunction("delay", [](lua_State *L) {
        const auto delay = static_cast<float>(luaL_checknumber(L, 1));
        luaL_checktype(L, 2, LUA_TFUNCTION);
        const int function = g_Lua->CreateReference();
        // using another vector because we might want to queue another delayed function when executing delayed functions
        // and we don't want to add items when iterating the vector
        g_Lua->m_queuedDelayedLuaFunctions.emplace_back(delay, function);
        return 0;
    });

    SetGlobalFunction("playAudio", [](lua_State *L) {
        const std::string path = luaL_checkstring(L, 1);
        g_Audio->PlayOneShot(path);
        return 0;
    });
}

void GameLua::Update(float deltaTime) {
    std::vector<std::tuple<float, int>> delayedLuaFunctions = std::move(m_queuedDelayedLuaFunctions);
    for (auto [delay, function]: m_delayedLuaFunctions) {
        delay -= deltaTime;
        if (delay <= 0.0f) {
            PushReference(function);
            PCall(0, 0);
            FreeReference(function);
        } else {
            delayedLuaFunctions.emplace_back(delay, function);
        }
    }
    m_delayedLuaFunctions = std::move(delayedLuaFunctions);
}
