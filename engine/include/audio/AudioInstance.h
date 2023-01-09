//
// Created by andyroiiid on 1/8/2023.
//

#pragma once

#include <glm/vec3.hpp>

namespace FMOD::Studio {
class EventDescription;

class EventInstance;
} // namespace FMOD::Studio

class AudioInstance {
public:
    AudioInstance() = default;

    explicit AudioInstance(FMOD::Studio::EventDescription *event);

    ~AudioInstance() { Release(); }

    AudioInstance(const AudioInstance &) = delete;

    AudioInstance &operator=(const AudioInstance &) = delete;

    AudioInstance(AudioInstance &&other) noexcept { Swap(other); }

    AudioInstance &operator=(AudioInstance &&other) noexcept {
        if (this != &other) {
            Release();
            Swap(other);
        }
        return *this;
    }

    void Release();

    void Swap(AudioInstance &other) noexcept;

    void Start();

    void Stop();

    void Set3DAttributes(
        const glm::vec3 &position,
        const glm::vec3 &velocity = {0.0f, 0.0f, 0.0f},
        const glm::vec3 &forward  = {0.0f, 0.0f, 1.0f},
        const glm::vec3 &up       = {0.0f, 1.0f, 0.0f}
    );

private:
    FMOD::Studio::EventInstance *m_instance = nullptr;
};
