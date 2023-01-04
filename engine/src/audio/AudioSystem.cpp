//
// Created by andyroiiid on 1/3/2023.
//

#include "audio/AudioSystem.h"

#include <core/Debug.h>
#include <fmod_studio.hpp>
#include <fmod_errors.h>

template<typename... Args>
bool DebugCheckFMOD(const FMOD_RESULT result, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    return DebugCheck(result == FMOD_OK, failMessage, args..., FMOD_ErrorString(result));
}

template<typename... Args>
void DebugCheckCriticalFMOD(const FMOD_RESULT result, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    DebugCheckCritical(result == FMOD_OK, failMessage, args..., FMOD_ErrorString(result));
}

AudioSystem::AudioSystem() {
    DebugCheckCriticalFMOD(FMOD::Studio::System::create(&m_system), "Failed to create FMOD system: {}");
    DebugCheckCriticalFMOD(m_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr), "Failed to initialize FMOD system: {}");
    DebugInfo("FMOD {:x}.{:x}.{:x}", FMOD_VERSION >> 16, (FMOD_VERSION >> 8) & 0xFF, FMOD_VERSION & 0xFF);
}

AudioSystem::~AudioSystem() {
    if (m_system) {
        DebugCheckCriticalFMOD(m_system->release(), "Failed to release FMOD system: {}");
    }
}

void AudioSystem::Update() {
    DebugCheckFMOD(m_system->update(), "Failed to update FMOD system.");
}
