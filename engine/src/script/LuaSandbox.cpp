//
// Created by andyroiiid on 1/1/2023.
//

#include "script/LuaSandbox.h"

#include "core/Debug.h"
#include "file/FileSystem.h"

int LuaPrint(lua_State *L) {
    const char *message = luaL_checkstring(L, 1);
    DebugInfo("Lua: {}", message);
    return 0;
}

LuaSandbox::LuaSandbox() {
    L = luaL_newstate();
    DebugCheckCritical(L, "Failed to create Lua sandbox.");
    DebugInfo("{}", LUA_RELEASE);

    SetGlobalFunction("print", LuaPrint);
}

LuaSandbox::~LuaSandbox() {
    lua_close(L);
}

void LuaSandbox::DebugStackItem(int idx) {
    const int type = lua_type(L, idx);
    switch (type) {
        case LUA_TNONE:
            DebugInfo("[#{}: None]", idx);
            break;
        case LUA_TNIL:
            DebugInfo("[#{}: Nil]", idx);
            break;
        case LUA_TBOOLEAN:
            DebugInfo("[#{}: {}]", idx, lua_toboolean(L, idx) ? "True" : "False");
            break;
        case LUA_TLIGHTUSERDATA:
            DebugInfo("[#{}: <LightUserData {}>", idx, lua_topointer(L, idx));
            break;
        case LUA_TNUMBER:
            DebugInfo("[#{}: {}", idx, lua_tonumber(L, idx));
            break;
        case LUA_TSTRING:
            DebugInfo("[#{}: {}", idx, lua_tostring(L, idx));
            break;
        case LUA_TTABLE:
            DebugInfo("[#{}: <Table {}>", idx, lua_topointer(L, idx));
            break;
        case LUA_TFUNCTION:
            DebugInfo("[#{}: <Function {}>", idx, lua_topointer(L, idx));
            break;
        case LUA_TUSERDATA:
            DebugInfo("[#{}: <UserData {}>", idx, lua_topointer(L, idx));
            break;
        case LUA_TTHREAD:
            DebugInfo("[#{}: <Thread {}>", idx, lua_topointer(L, idx));
            break;
        default:
            DebugWarning("[#{}: Unknown]", idx);
            break;
    }
}

void LuaSandbox::DebugStack() {
    const int top = lua_gettop(L);
    DebugInfo("Lua stack ({} items):", top);
    for (int i = 1; i <= top; i++) {
        DebugStackItem(i);
    }
}

void LuaSandbox::SetGlobalFunction(const std::string &name, lua_CFunction function) {
    lua_pushcfunction(L, function);
    lua_setglobal(L, name.c_str());
}

void LuaSandbox::CallGlobalFunction(const std::string &name) {
    const int type = lua_getglobal(L, name.c_str());

    if (type != LUA_TFUNCTION) {
        DebugError("{} is not a Lua function.", name);
        lua_pop(L, 1);
        return;
    }

    PCall();
}

void LuaSandbox::DoFile(const std::string &filename) {
    const std::string code = FileSystem::Read(filename);

    if (luaL_loadbuffer(L, code.data(), code.size(), filename.c_str()) != LUA_OK) {
        DebugError("Failed to load Lua script: {}", lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }

    PCall();
}

void LuaSandbox::PCall() {
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        DebugError("Failed to execute Lua script: {}", lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }
}
