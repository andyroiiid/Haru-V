//
// Created by andyroiiid on 12/27/2022.
//

#pragma once

#include <string>
#include <math/Transform.h>

namespace physx {
    struct PxRaycastHit;
}

struct lua_State;

class APlayer;

#define DEFINE_ACTOR_CLASS(className) \
    className(const className&) = delete; \
    className& operator=(const className&) = delete; \
    className(className&&) = delete; \
    className& operator=(className&&) = delete; \
    static inline const std::string &ClassName = #className; \
    const std::string &GetActorClassName() const override { return ClassName; }

class Actor {
public:
    Actor() = default;

    virtual ~Actor() = default;

    Actor(const Actor &) = delete;

    Actor &operator=(const Actor &) = delete;

    Actor(Actor &&) = delete;

    Actor &operator=(Actor &&) = delete;

    [[nodiscard]] virtual const std::string &GetActorClassName() const = 0;

    template<class T>
    [[nodiscard]] bool IsClass() const {
        return T::ClassName == GetActorClassName();
    }

    template<class T>
    T *Cast() {
        return IsClass<T>() ? reinterpret_cast<T *>(this) : nullptr;
    }

    virtual void Update(float deltaTime) {}

    virtual void FixedUpdate(float fixedDeltaTime) {}

    virtual void Draw() {}

    virtual void LuaSignal(lua_State *L) {}

    virtual void Use(APlayer *player, const physx::PxRaycastHit &hit) {}

    [[nodiscard]] bool IsPendingDestroy() const { return m_pendingDestroy; }

    void Destroy() { m_pendingDestroy = true; }

    const Transform &GetTransform() const { return m_transform; }

    Transform &GetTransform() { return m_transform; }

private:
    bool m_pendingDestroy = false;
    Transform m_transform;
};
