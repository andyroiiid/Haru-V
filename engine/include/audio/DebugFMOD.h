//
// Created by andyroiiid on 1/8/2023.
//

#pragma once

#include "core/Debug.h"

#include <fmod_errors.h>

template<typename... Args>
bool DebugCheckFMOD(const FMOD_RESULT result, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    return DebugCheck(result == FMOD_OK, failMessage, args..., FMOD_ErrorString(result));
}

template<typename... Args>
void DebugCheckCriticalFMOD(const FMOD_RESULT result, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    DebugCheckCritical(result == FMOD_OK, failMessage, args..., FMOD_ErrorString(result));
}
