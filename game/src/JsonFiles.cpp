//
// Created by andyroiiid on 12/21/2022.
//

#include "JsonFiles.h"

#include <file/JsonFile.h>

GameConfig::GameConfig(const std::string &jsonFilename) {
    JsonFile json(jsonFilename);
    Name = json.GetString("name");
    Width = json.GetInteger("width");
    Height = json.GetInteger("height");
    Maximized = json.GetBoolean("maximized");
}

PbrMaterialConfig::PbrMaterialConfig(const std::string &jsonFilename) {
    JsonFile json(jsonFilename);
    Albedo = json.GetString("albedo");
    Normal = json.GetString("normal");
    MRA = json.GetString("mra");
    Emissive = json.GetString("emissive");
}
