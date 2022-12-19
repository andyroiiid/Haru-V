//
// Created by andyroiiid on 12/18/2022.
//

#pragma once

#include <simdjson.h>

#include "core/Debug.h"
#include "file/FileSystem.h"

template<class T>
T UnpackJsonValue(simdjson::ondemand::value &value, const std::string &name) {
    T result;
    simdjson::error_code error = value.get(result);
    DebugCheckCritical(
            error == simdjson::SUCCESS,
            "Failed to unpack JSON value '{}': {}",
            name.c_str(),
            simdjson::error_message(error)
    );
    return result;
}

class JsonFile {
public:
    explicit JsonFile(const std::string &filename) : m_json(FileSystem::Read(filename)) {
        simdjson::error_code error = m_parser.iterate(m_json).get(m_document);
        DebugCheckCritical(
                error == simdjson::SUCCESS,
                "Failed to load json document: {}",
                simdjson::error_message(error)
        );
    }

    ~JsonFile() = default;

    JsonFile(const JsonFile &) = delete;

    JsonFile &operator=(const JsonFile &) = delete;

    JsonFile(JsonFile &&) = delete;

    JsonFile &operator=(JsonFile &&) = delete;

    template<class T>
    T GetField(const std::string &key) {
        simdjson::ondemand::value value;
        simdjson::error_code error = m_document.find_field(key).get(value);
        DebugCheckCritical(
                error == simdjson::SUCCESS,
                "Failed to find key '{}': {}",
                key,
                simdjson::error_message(error)
        );
        return UnpackJsonValue<T>(value, key);
    }

    template<>
    std::string GetField(const std::string &key) {
        return std::string(GetField<std::string_view>(key));
    }

    template<>
    int GetField(const std::string &key) {
        return static_cast<int>(GetField<double>(key));
    }

private:
    simdjson::padded_string m_json;

    // Try to reuse this: https://github.com/simdjson/simdjson/blob/master/doc/basics.md#parser-document-and-json-scope
    simdjson::ondemand::parser m_parser;

    simdjson::ondemand::document m_document;
};
