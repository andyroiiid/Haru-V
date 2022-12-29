//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <vector>

class BinaryParser {
public:
    explicit BinaryParser(const std::string &filename);

    BinaryParser(const BinaryParser &) = delete;

    BinaryParser &operator=(const BinaryParser &) = delete;

    BinaryParser(BinaryParser &&) = delete;

    BinaryParser &operator=(BinaryParser &&) = delete;

protected:
    bool ReadU8(uint8_t &value) { return ReadValue(value); }

    bool ReadU16(uint16_t &value) { return ReadValue(value); }

    bool ReadFloat(float &value) { return ReadValue(value); }

    bool ReadVec3(glm::vec3 &value) { return ReadValue(value); }

    // u8 length; char string[length];
    bool ReadShortString(std::string &string) { return ReadString<uint8_t>(string); }

    // u16 size; T vector[size];
    template<class T>
    bool ReadVector(std::vector<T> &vector) { return ReadVectorImpl<uint16_t>(vector); }

private:
    bool ReadBytes(size_t numBytes, void *output);

    template<class T>
    bool ReadValue(T &value) {
        return ReadBytes(sizeof(value), &value);
    }

    template<class LengthType>
    bool ReadString(std::string &string) {
        LengthType length;
        if (!ReadValue(length)) {
            return false;
        }
        string.resize(length);
        return ReadBytes(length, string.data());
    }

    template<class LengthType, class ValueType>
    bool ReadVectorImpl(std::vector<ValueType> &vector) {
        LengthType size;
        if (!ReadValue(size)) {
            return false;
        }
        vector.resize(size);
        return ReadBytes(size * sizeof(ValueType), vector.data());
    }

    std::string m_binary;

    const char *m_current;
    size_t m_remainingBytes;
};
