//
// Created by andyroiiid on 12/18/2022.
//

#pragma once

#include <string>

namespace FileSystem {
    void Init();

    void Shutdown();

    void Mount(const std::string &newDir, const std::string &mountPoint, bool appendToPath = true);

    std::string Read(const std::string &filename);
}
