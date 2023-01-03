//
// Created by andyroiiid on 12/27/2022.
//

#include "actors/Scene.h"

#include <lua.hpp>
#include <core/Debug.h>

#include "Globals.h"

void Scene::Update(const float deltaTime) {
    // put pending destroy actors from last frame into a temp queue
    const std::vector<std::unique_ptr<Actor>> pendingDestroyActorsFromLastFrame = std::move(m_pendingDestroyActors);

    // update all alive actors
    std::vector<std::unique_ptr<Actor>> aliveActors;
    aliveActors.reserve(m_actors.size());
    for (auto &actor: m_actors) {
        actor->Update(deltaTime);
        if (actor->IsPendingDestroy()) {
            // put pending destroy actors in this frame into m_pendingDestroyActors
            m_pendingDestroyActors.push_back(std::move(actor));
        } else {
            aliveActors.push_back(std::move(actor));
        }
    }
    m_actors = std::move(aliveActors);

    // pendingDestroyActorsFromLastFrame deconstructs and releases all pending destroy actors
    // destroy pending destroy actors from last frame
    // this strategy will give other actors one frame to cleanup their references
}

void Scene::FixedUpdate(float fixedDeltaTime) {
    for (auto &actor: m_actors) {
        actor->FixedUpdate(fixedDeltaTime);
    }
}

void Scene::Draw() {
    for (auto &actor: m_actors) {
        actor->Draw();
    }
}

Actor *Scene::FindFirstActorOfClassImpl(const std::string &className) const {
    for (auto &actor: m_actors) {
        if (actor->GetActorClassName() == className) {
            return actor.get();
        }
    }
    return nullptr;
}

Actor *Scene::FindActorWithName(const std::string &name) const {
    auto pair = m_registeredActors.find(name);
    if (pair == m_registeredActors.end()) {
        DebugError("Failed to find actor \"{}\"", name);
        return nullptr;
    }
    return pair->second;
}

void Scene::Register(const std::string &name, Actor *actor) {
    auto pair = m_registeredActors.find(name);
    if (pair == m_registeredActors.end()) {
        m_registeredActors.emplace(name, actor);
    } else {
        DebugError("Failed to register actor \"{}\" because the name is already registered.");
    }
}

int Scene::LuaSignal(lua_State *L) {
    const std::string name = luaL_checkstring(L, 1);
    Actor *actor = g_Scene->FindActorWithName(name);
    if (actor != nullptr) {
        actor->LuaSignal(L);
    }
    return 0;
}
