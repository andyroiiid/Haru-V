//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include <file/BinaryParser.h>

#include "map/MapData.h"

class MapParser : private BinaryParser {
public:
    explicit MapParser(const std::string &filename);

    [[nodiscard]] const MapData::Map &GetMap() const { return m_map; }

private:
    void ParseFace(MapData::Face &face);

    void ParseBrush(MapData::Brush &brush);

    void ParseEntity(MapData::Entity &entity);

    void ParseMap(MapData::Map &map);

    MapData::Map m_map;
};
