//
// Created by andyroiiid on 1/3/2023.
//

#pragma once

#include <string>

#include "audio/AudioInstance.h"

namespace FMOD::Studio {
class System;

class Bank;

class Bus;

class EventDescription;
} // namespace FMOD::Studio

class AudioSystem {
public:
    AudioSystem();

    ~AudioSystem();

    AudioSystem(const AudioSystem &) = delete;

    AudioSystem &operator=(const AudioSystem &) = delete;

    AudioSystem(AudioSystem &&) = delete;

    AudioSystem &operator=(AudioSystem &&) = delete;

    void Update();

    void StopAllEvents();

    void SetListener3DAttributes(
        const glm::vec3 &position,
        const glm::vec3 &velocity = {0.0f, 0.0f, 0.0f},
        const glm::vec3 &forward  = {0.0f, 0.0f, 1.0f},
        const glm::vec3 &up       = {0.0f, 1.0f, 0.0f}
    );

    FMOD::Studio::EventDescription *FindEvent(const std::string &path);

    AudioInstance CreateInstance(const std::string &path) { return AudioInstance(FindEvent(path)); }

    void PlayOneShot(const std::string &path) { CreateInstance(path).Start(); }

private:
    FMOD::Studio::Bank *LoadBank(const std::string &bankFilename);

    void UnloadBank(FMOD::Studio::Bank *bank);

    FMOD::Studio::System *m_system            = nullptr;
    FMOD::Studio::Bank   *m_masterBank        = nullptr;
    FMOD::Studio::Bank   *m_masterStringsBank = nullptr;
    FMOD::Studio::Bus    *m_masterBus         = nullptr;
};
