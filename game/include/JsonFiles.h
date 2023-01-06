//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <string>

struct GameConfig {
    std::string Name;
    int         Width;
    int         Height;
    bool        Maximized;
    std::string StartMap;

    explicit GameConfig(const std::string &jsonFilename);
};

struct PbrMaterialConfig {
    std::string Albedo;
    std::string Normal;
    std::string MRA;
    std::string Emissive;
    bool        Transparent;
    bool        Shadow;

    explicit PbrMaterialConfig(const std::string &jsonFilename);
};
