//
// Created by andyroiiid on 12/18/2022.
//

#pragma once

#include <simdjson.h>

class JsonFile {
public:
    explicit JsonFile(const std::string &filename);

    ~JsonFile() = default;

    JsonFile(const JsonFile &) = delete;

    JsonFile &operator=(const JsonFile &) = delete;

    JsonFile(JsonFile &&) = delete;

    JsonFile &operator=(JsonFile &&) = delete;

    std::string GetString(const std::string &key);

    std::string GetString(const std::string &key, const std::string &fallback);

    int GetInteger(const std::string &key);

    int GetInteger(const std::string &key, int fallback);

    bool GetBoolean(const std::string &key);

    bool GetBoolean(const std::string &key, bool fallback);

private:
    template<class T>
    T GetCriticalField(const std::string &key);

    template<class T>
    T GetField(const std::string &key, const T &fallback);

    simdjson::dom::parser m_parser;

    simdjson::dom::element m_document;
};
