//
// Created by andyroiiid on 12/28/2022.
//

#include "map/MapData.h"

#include <sstream>

// TODO: use custom parsing functions instead of string streams

const std::string &MapData::Entity::GetProperty(const std::string &key, const std::string &fallback) const {
    auto pair = Properties.find(key);
    if (pair == Properties.end()) {
        return fallback;
    }
    return pair->second;
}

bool MapData::Entity::GetPropertyString(const std::string &key, std::string &value) const {
    value = GetProperty(key);
    return !value.empty();
}

bool MapData::Entity::GetPropertyInteger(const std::string &key, int &value) const {
    const std::string &literal = GetProperty(key);
    if (literal.empty()) return false;

    std::stringstream literalStream(literal);
    int x;
    literalStream >> x;
    if (literalStream.bad()) return false;

    value = x;
    return true;
}

bool MapData::Entity::GetPropertyFloat(const std::string &key, float &value) const {
    const std::string &literal = GetProperty(key);
    if (literal.empty()) return false;

    std::stringstream literalStream(literal);
    float x;
    literalStream >> x;
    if (literalStream.bad()) return false;

    value = x;
    return true;
}

bool MapData::Entity::GetPropertyColor(const std::string &key, glm::vec3 &value) const {
    return GetPropertyVec3(key, value);
}

bool MapData::Entity::GetPropertyVector(const std::string &key, glm::vec3 &value) const {
    if (!GetPropertyVec3(key, value)) return false;
    // convert from quake direction to haru direction
    std::swap(value.y, value.z);
    value /= 32.0f;
    return true;
}

bool MapData::Entity::GetPropertyVec3(const std::string &key, glm::vec3 &value) const {
    const std::string &literal = GetProperty(key);
    if (literal.empty()) return false;

    std::stringstream literalStream(literal);
    float x, y, z;
    literalStream >> x >> y >> z;
    if (literalStream.bad()) return false;

    value = {x, y, z};
    return true;
}
