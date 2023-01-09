//
// Created by andyroiiid on 1/3/2023.
//

#include "audio/AudioSystem.h"

#include <fmod_studio.hpp>

#include "audio/DebugFMOD.h"
#include "file/FileSystem.h"

AudioSystem::AudioSystem() {
    DebugCheckCriticalFMOD(FMOD::Studio::System::create(&m_system), "Failed to create FMOD system: {}");
    DebugCheckCriticalFMOD(m_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr), "Failed to initialize FMOD system: {}");
    DebugInfo("FMOD {:x}.{:x}.{:x}", FMOD_VERSION >> 16, (FMOD_VERSION >> 8) & 0xFF, FMOD_VERSION & 0xFF);

    m_masterBank        = LoadBank("audio/Master.bank");
    m_masterStringsBank = LoadBank("audio/Master.strings.bank");

    DebugCheckCriticalFMOD(m_system->getBus("bus:/", &m_masterBus), "Failed to get master FMOD bus: {}");
}

AudioSystem::~AudioSystem() {
    UnloadBank(m_masterStringsBank);
    UnloadBank(m_masterBank);

    if (m_system) {
        DebugCheckCriticalFMOD(m_system->release(), "Failed to release FMOD system: {}");
    }
}

FMOD::Studio::Bank *AudioSystem::LoadBank(const std::string &bankFilename) {
    DebugInfo("Loading FMOD bank {}", bankFilename);

    std::string         bankFile = FileSystem::Read(bankFilename);
    FMOD::Studio::Bank *bank     = nullptr;
    DebugCheckFMOD(
        m_system->loadBankMemory(bankFile.data(), static_cast<int>(bankFile.size()), FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank),
        "Failed to load FMOD bank {}: {}",
        bankFilename
    );
    return bank;
}

void AudioSystem::UnloadBank(FMOD::Studio::Bank *bank) { // NOLINT(readability-convert-member-functions-to-static)
    DebugCheckFMOD(bank->unload(), "Failed to unload FMOD bank: {}");
}

void AudioSystem::Update() {
    DebugCheckFMOD(m_system->update(), "Failed to update FMOD system: {}");
}

void AudioSystem::StopAllEvents() {
    DebugCheckFMOD(m_masterBus->stopAllEvents(FMOD_STUDIO_STOP_ALLOWFADEOUT), "Failed to stop all FMOD events: {}");
}

void AudioSystem::SetListener3DAttributes(const glm::vec3 &position, const glm::vec3 &velocity, const glm::vec3 &forward, const glm::vec3 &up) {
    const FMOD_3D_ATTRIBUTES attributes{
        {position.x, position.y, position.z},
        {velocity.x, velocity.y, velocity.z},
        {forward.x,  forward.y,  forward.z },
        {up.x,       up.y,       up.z      }
    };
    DebugCheckFMOD(m_system->setListenerAttributes(0, &attributes), "Failed to set 3d attributes on FMOD listener: {}");
}

FMOD::Studio::EventDescription *AudioSystem::FindEvent(const std::string &path) {
    FMOD::Studio::EventDescription *event = nullptr;
    DebugCheckFMOD(m_system->getEvent(path.c_str(), &event), "Cannot find FMOD event {}: {}", path);
    return event;
}
