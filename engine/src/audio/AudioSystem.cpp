//
// Created by andyroiiid on 1/3/2023.
//

#include "audio/AudioSystem.h"

#include <fmod_studio.hpp>
#include <fmod_errors.h>

#include "core/Debug.h"
#include "file/FileSystem.h"

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

FMOD::Studio::Bank *AudioSystem::LoadBank(const std::string &bankFilename) {
    DebugInfo("Loading FMOD bank {}", bankFilename);

    std::string bankFile = FileSystem::Read(bankFilename);
    FMOD::Studio::Bank *bank = nullptr;
    DebugCheckFMOD(
            m_system->loadBankMemory(
                    bankFile.data(),
                    static_cast<int>(bankFile.size()),
                    FMOD_STUDIO_LOAD_MEMORY,
                    FMOD_STUDIO_LOAD_BANK_NORMAL,
                    &bank
            ),
            "Failed to load FMOD bank {}: {}",
            bankFilename
    );
    return bank;
}

void AudioSystem::UnloadBank(FMOD::Studio::Bank *bank) { // NOLINT(readability-convert-member-functions-to-static)
    DebugCheckFMOD(bank->unload(), "Failed to unload FMOD bank: {}");
}

FMOD::Studio::EventDescription *AudioSystem::FindEvent(const std::string &path) {
    FMOD::Studio::EventDescription *event = nullptr;
    DebugCheckFMOD(m_system->getEvent(path.c_str(), &event), "Cannot find FMOD event {}: {}", path);
    return event;
}

void AudioSystem::PlayOneShot(FMOD::Studio::EventDescription *event) { // NOLINT(readability-convert-member-functions-to-static)
    FMOD::Studio::EventInstance *instance = nullptr;

    if (!DebugCheckFMOD(event->createInstance(&instance), "Failed to create FMOD event instance: {}")) {
        return;
    }

    if (!DebugCheckFMOD(instance->start(), "Failed to start FMOD event instance: {}")) {
        return;
    }

    if (!DebugCheckFMOD(instance->release(), "Failed to release FMOD event instance: {}")) {
        return;
    }
}
