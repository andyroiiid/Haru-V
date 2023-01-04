//
// Created by andyroiiid on 12/27/2022.
//

#pragma once

#include <memory>
#include <vector>
#include <map>

#include "actors/Actor.h"

class Scene {
public:
    Scene() = default;

    ~Scene() = default;

    Scene(const Scene &) = delete;

    Scene &operator=(const Scene &) = delete;

    Scene(Scene &&) = delete;

    Scene &operator=(Scene &&) = delete;

    void Update(float deltaTime);

    void FixedUpdate(float fixedDeltaTime);

    void Draw();

    template<class T, class... Args, std::enable_if_t<!std::is_array_v<T>, int>  = 0>
    T *CreateActor(Args &&... args) {
        std::unique_ptr<T> actor = std::make_unique<T>(std::forward<Args>(args)...);
        T *actorRef = actor.get();
        m_actors.emplace_back(std::move(actor));
        return actorRef;
    }

    template<class T>
    [[nodiscard]] T *FindFirstActorOfClass() const {
        return reinterpret_cast<T *>(FindFirstActorOfClassImpl(T::ClassName));
    }

    void Register(const std::string &name, Actor *actor);

    [[nodiscard]] Actor *FindActorWithName(const std::string &name) const;

private:
    [[nodiscard]] Actor *FindFirstActorOfClassImpl(const std::string &className) const;

    std::vector<std::unique_ptr<Actor>> m_actors;
    std::vector<std::unique_ptr<Actor>> m_pendingDestroyActors;

    std::map<std::string, Actor *> m_registeredActors;
};
