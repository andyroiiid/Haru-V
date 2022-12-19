//
// Created by andyroiiid on 12/18/2022.
//

#include "file/FileSystem.h"

#include <physfs.h>

#include "core/Debug.h"

static const char *GetLastError() {
    return PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
}

void FileSystem::Init() {
    DebugCheckCritical(PHYSFS_init(__argv[0]), "Failed to init PhysFS: {}", GetLastError());
    DebugInfo("PhysFS {}.{}.{}", PHYSFS_VER_MAJOR, PHYSFS_VER_MINOR, PHYSFS_VER_PATCH);
}

void FileSystem::Shutdown() {
    DebugCheckCritical(PHYSFS_deinit(), "Failed to shutdown PhysFS: {}", GetLastError());
}

void FileSystem::Mount(const std::string &newDir, const std::string &mountPoint, bool appendToPath) {
    if (!PHYSFS_mount(newDir.c_str(), mountPoint.c_str(), appendToPath)) {
        DebugError("PhysFS failed to mount {} at {}: {}", newDir.c_str(), mountPoint.c_str(), GetLastError());
    } else {
        DebugInfo("PhysFS mount {} at {}", newDir.c_str(), mountPoint.c_str());
    }
}

std::string FileSystem::Read(const std::string &filename) {
    PHYSFS_File *file = PHYSFS_openRead(filename.c_str());
    if (!file) {
        DebugError("PhysFS failed to open file {}: {}", filename.c_str(), GetLastError());
        return {};
    }
    PHYSFS_sint64 length = PHYSFS_fileLength(file);
    if (length == -1) {
        DebugError("PhysFS failed to get file size {}: {}", filename.c_str(), GetLastError());
        return {};
    }
    std::string bytes(length, '\0');
    if (PHYSFS_readBytes(file, bytes.data(), length) == -1) {
        DebugError("PhysFS failed to read file {}: {}", filename.c_str(), GetLastError());
    }
    return bytes;
}
