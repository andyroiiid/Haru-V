//
// Created by andyroiiid on 12/28/2022.
//

#include "file/BinaryParser.h"

#include "core/Debug.h"
#include "file/FileSystem.h"

BinaryParser::BinaryParser(const std::string &filename)
    : m_binary(FileSystem::Read(filename))
    , m_current(m_binary.data())
    , m_remainingBytes(m_binary.size()) {
}

bool BinaryParser::ReadBytes(size_t numBytes, void *output) {
    if (m_remainingBytes < numBytes) {
        DebugError("Not enough data in the binary file.");
        return false;
    }
    memcpy(output, m_current, numBytes);
    m_current        += numBytes;
    m_remainingBytes -= numBytes;
    return true;
}
