//
// Created by andyroiiid on 1/3/2023.
//

#pragma once

#include <string>

namespace FMOD::Studio {
class System;

class Bank;

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

    FMOD::Studio::Bank *LoadBank(const std::string &bankFilename);

    void UnloadBank(FMOD::Studio::Bank *bank);

    FMOD::Studio::EventDescription *FindEvent(const std::string &path);

    void PlayOneShot(FMOD::Studio::EventDescription *event);

    void PlayOneShot(const std::string &path) { PlayOneShot(FindEvent(path)); }

private:
    FMOD::Studio::System *m_system = nullptr;
};
