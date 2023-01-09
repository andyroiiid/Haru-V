//
// Created by andyroiiid on 1/8/2023.
//
#include "audio/AudioInstance.h"

#include <fmod_studio.hpp>

#include "audio/DebugFMOD.h"

AudioInstance::AudioInstance(FMOD::Studio::EventDescription *event) {
    DebugCheckFMOD(event->createInstance(&m_instance), "Failed to create FMOD event instance: {}");
}

void AudioInstance::Release() {
    if (!m_instance) {
        return;
    }

    DebugCheckFMOD(m_instance->release(), "Failed to release FMOD event instance: {}");
    m_instance = nullptr;
}

void AudioInstance::Swap(AudioInstance &other) noexcept {
    std::swap(m_instance, other.m_instance);
}

void AudioInstance::Start() {
    if (!m_instance) {
        return;
    }

    DebugCheckFMOD(m_instance->start(), "Failed to start FMOD event instance: {}");
}

void AudioInstance::Stop() {
    if (!m_instance) {
        return;
    }

    DebugCheckFMOD(m_instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT), "Failed to stop FMOD event instance: {}");
}

void AudioInstance::Set3DAttributes(const glm::vec3 &position, const glm::vec3 &velocity, const glm::vec3 &forward, const glm::vec3 &up) {
    if (!m_instance) {
        return;
    }

    const FMOD_3D_ATTRIBUTES attributes{
        {position.x, position.y, position.z},
        {velocity.x, velocity.y, velocity.z},
        {forward.x,  forward.y,  forward.z },
        {up.x,       up.y,       up.z      }
    };
    DebugCheckFMOD(m_instance->set3DAttributes(&attributes), "Failed to set 3d attributes on FMOD event instance: {}");
}
