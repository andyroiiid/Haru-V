//
// Created by andyroiiid on 1/1/2023.
//

#include "script/LuaSandbox.h"

#include "core/Debug.h"
#include "file/FileSystem.h"

static int LuaPrint(lua_State *L) {
    const char *message = luaL_checkstring(L, 1);
    DebugInfo("{}", message);
    return 0;
}

static int LuaPackageSearcher(lua_State *L) {
    const std::string path = luaL_checkstring(L, 1);
    DebugInfo("Loading Lua package: {}", path);
    const std::string source = FileSystem::Read(path);
    luaL_loadbuffer(L, source.data(), source.size(), path.c_str());
    return 1;
}

LuaSandbox::LuaSandbox() {
    L = luaL_newstate();
    DebugCheckCritical(L, "Failed to create Lua sandbox.");
    DebugInfo("{}", LUA_RELEASE);

    luaL_openlibs(L);

    SetupPackageSearcher();

    SetGlobalFunction("print", LuaPrint);
}

LuaSandbox::~LuaSandbox() {
    lua_close(L);
}

void LuaSandbox::SetupPackageSearcher() {
    //
    lua_getglobal(L, "package");
    // -1: package
    lua_getfield(L, -1, "searchers");
    // -2: package, -1: packager.searchers

    if (lua_type(L, -1) != LUA_TTABLE) {
        // -2: package, -1: packager.searchers (not table)
        lua_pop(L, 1);
        // -1: package
        lua_createtable(L, 1, 0);
        // -2: package, -1: {}
        lua_pushvalue(L, -1);
        // -3: package, -2: {}, -1: {}
        lua_setfield(L, -3, "searchers");
        // -2: package, -1: package.searchers
    }

    lua_Integer n = luaL_len(L, -1);
    lua_pushcfunction(L, LuaPackageSearcher);
    // -3: package, -2: package.searchers, -1: LuaPackageSearcher
    lua_seti(L, -2, n + 1);
    // -2: package, -1: packager.searchers

    lua_pop(L, 2);
    //
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

    PCall(0, 0);
}

void LuaSandbox::CallGlobalFunction(const std::string &name, const std::string &arg) {
    const int type = lua_getglobal(L, name.c_str());

    if (type != LUA_TFUNCTION) {
        DebugError("{} is not a Lua function.", name);
        lua_pop(L, 1);
        return;
    }

    lua_pushstring(L, arg.c_str());

    PCall(1, 0);
}

void LuaSandbox::DoSource(const std::string &source, const std::string &name) {
    DebugInfo("Executing Lua script {}", name);

    if (luaL_loadbuffer(L, source.data(), source.size(), name.c_str()) != LUA_OK) {
        DebugError("Failed to load Lua script: {}", lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }

    PCall(0, 0);
}

void LuaSandbox::DoFile(const std::string &filename) {
    const std::string source = FileSystem::Read(filename);
    DoSource(source, filename);
}

void LuaSandbox::PCall(int nArgs, int nResults) {
    if (lua_pcall(L, nArgs, nResults, 0) != LUA_OK) {
        DebugError("Failed to execute Lua script: {}", lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }
}
