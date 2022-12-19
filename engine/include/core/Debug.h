//
// Created by andyroiiid on 12/18/2022.
//

#pragma once

#include <spdlog/spdlog.h>

template<typename... Args>
void DebugVerbose(const spdlog::format_string_t<Args...> &format, Args... args) {
    spdlog::trace(format, args...);
}

template<typename... Args>
void DebugInfo(const spdlog::format_string_t<Args...> &format, Args... args) {
    spdlog::info(format, args...);
}

template<typename... Args>
void DebugWarning(const spdlog::format_string_t<Args...> &format, Args... args) {
    spdlog::warn(format, args...);
}

template<typename... Args>
void DebugError(const spdlog::format_string_t<Args...> &format, Args... args) {
    spdlog::error(format, args...);
}

template<typename... Args>
bool DebugCheck(const bool succeeded, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    if (succeeded) return true;
    DebugWarning(failMessage, args...);
    return false;
}

template<typename... Args>
void DebugCheckCritical(const bool succeeded, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    if (succeeded) return;
    DebugError(failMessage, args...);
    exit(EXIT_FAILURE);
}
