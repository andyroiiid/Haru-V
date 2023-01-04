//
// Created by andyroiiid on 1/3/2023.
//

#pragma once

namespace FMOD::Studio {
    class System;
}

class AudioSystem {
public:
    AudioSystem();

    ~AudioSystem();

    AudioSystem(const AudioSystem &) = delete;

    AudioSystem &operator=(const AudioSystem &) = delete;

    AudioSystem(AudioSystem &&) = delete;

    AudioSystem &operator=(AudioSystem &&) = delete;

    void Update();

private:
    FMOD::Studio::System *m_system = nullptr;
};
