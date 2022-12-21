//
// Created by andyroiiid on 12/18/2022.
//

#pragma once

#include "core/Debug.h"

#include <vulkan/vulkan.hpp>

template<typename... Args>
void DebugCheckVk(const VkResult result, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    DebugCheck(result == VK_SUCCESS, failMessage, args...);
}

template<typename... Args>
void DebugCheckVk(const vk::Result result, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    DebugCheck(result == vk::Result::eSuccess, failMessage, args...);
}

template<typename... Args>
void DebugCheckCriticalVk(const VkResult result, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    DebugCheckCritical(result == VK_SUCCESS, failMessage, args...);
}

template<typename... Args>
void DebugCheckCriticalVk(const vk::Result result, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    DebugCheckCritical(result == vk::Result::eSuccess, failMessage, args...);
}
