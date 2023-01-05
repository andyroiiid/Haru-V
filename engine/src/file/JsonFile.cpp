//
// Created by andyroiiid on 12/22/2022.
//

#include "file/JsonFile.h"

#include "core/Debug.h"
#include "file/FileSystem.h"

template<typename... Args>
void DebugCheckCriticalSimdJson(const simdjson::error_code error, const spdlog::format_string_t<Args...> &failMessage, Args... args) {
    DebugCheckCritical(error == simdjson::SUCCESS, failMessage, args..., simdjson::error_message(error));
}

JsonFile::JsonFile(const std::string &filename) {
    const simdjson::error_code error = m_parser.parse(FileSystem::Read(filename)).get(m_document);
    DebugCheckCriticalSimdJson(error, "Failed to load json document: {}");
}

template<class T>
T JsonFile::GetCriticalField(const std::string &key) {
    T                          value{};
    const simdjson::error_code error = m_document.at_key(key).get(value);
    DebugCheckCriticalSimdJson(error, "Failed to find key '{}': {}", key);
    return value;
}

template<class T>
T JsonFile::GetField(const std::string &key, const T &fallback) {
    T                          value{};
    const simdjson::error_code error = m_document.at_key(key).get(value);
    return error == simdjson::SUCCESS ? value : fallback;
}

std::string JsonFile::GetString(const std::string &key) {
    return std::string(GetCriticalField<std::string_view>(key));
}

std::string JsonFile::GetString(const std::string &key, const std::string &fallback) {
    return std::string(GetField<std::string_view>(key, fallback));
}

int JsonFile::GetInteger(const std::string &key) {
    return static_cast<int>(GetCriticalField<double>(key));
}

int JsonFile::GetInteger(const std::string &key, int fallback) {
    return static_cast<int>(GetField<double>(key, fallback));
}

bool JsonFile::GetBoolean(const std::string &key) {
    return GetCriticalField<bool>(key);
}

bool JsonFile::GetBoolean(const std::string &key, bool fallback) {
    return GetField(key, fallback);
}
