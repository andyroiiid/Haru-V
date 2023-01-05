//
// Created by andyroiiid on 12/28/2022.
//
#include "map/MapParser.h"

#include "core/Debug.h"

MapParser::MapParser(const std::string &filename)
    : BinaryParser(filename) {
    ParseMap(m_map);
}

void MapParser::ParseFace(MapData::Face &face) {
    DebugCheckCritical(ReadShortString(face.Texture), "Failed to parse Face::texture.");
    DebugCheckCritical(ReadVec3(face.Normal), "Failed to parse Face::normal.");
    DebugCheckCritical(ReadVector(face.Vertices), "Failed to parse Face::numVertices and Face::vertices.");
}

void MapParser::ParseBrush(MapData::Brush &brush) {
    DebugCheckCritical(ReadVector(brush.Vertices), "Failed to parse Brush::numVertices and Brush::vertices.");

    uint16_t numFaces;
    DebugCheckCritical(ReadU16(numFaces), "Failed to parse Brush::numFaces.");
    brush.Faces.resize(numFaces);
    for (int i = 0; i < numFaces; i++) {
        ParseFace(brush.Faces[i]);
    }
}

void MapParser::ParseEntity(MapData::Entity &entity) {
    uint16_t numProperties;
    DebugCheckCritical(ReadU16(numProperties), "Failed to parse Entity::numProperties.");

    std::string key;
    std::string value;
    for (int i = 0; i < numProperties; i++) {
        DebugCheckCritical(ReadShortString(key), "Failed to parse KeyValue::key.");
        DebugCheckCritical(ReadShortString(value), "Failed to parse KeyValue::value.");
        entity.Properties.emplace(key, value);
    }

    uint16_t numBrushes;
    DebugCheckCritical(ReadU16(numBrushes), "Failed to parse Entity::numBrushes.");
    entity.Brushes.resize(numBrushes);
    for (int i = 0; i < numBrushes; i++) {
        ParseBrush(entity.Brushes[i]);
    }
}

void MapParser::ParseMap(MapData::Map &map) {
    uint16_t numEntities;
    DebugCheckCritical(ReadU16(numEntities), "Failed to parse Map::numEntities.");
    map.Entities.resize(numEntities);
    for (int i = 0; i < numEntities; i++) {
        ParseEntity(map.Entities[i]);
    }
}
