//
// Created by andyroiiid on 12/21/2022.
//

#include "JsonFiles.h"

#include "file/JsonFile.h"

GameConfig::GameConfig(const std::string &jsonFilename) {
    JsonFile gameJson(jsonFilename);
    Name = gameJson.GetField<std::string>("name");
    Width = gameJson.GetField<int>("width");
    Height = gameJson.GetField<int>("height");
}
