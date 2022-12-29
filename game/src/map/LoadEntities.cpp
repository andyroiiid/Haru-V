//
// Created by andyroiiid on 12/28/2022.
//

#include "map/LoadEntities.h"

#include <core/Debug.h>

#include "map/MapParser.h"

void LoadEntities(const std::string &mapFilename) {
    const MapParser mapParser(mapFilename);
    LoadEntities(mapParser.GetMap());
}

void LoadEntities(const MapData::Map &map) {
    for (const MapData::Entity &entity: map.Entities) {
        DebugInfo("BEGIN ENTITY");
        for (const auto &[key, value]: entity.Properties) {
            DebugInfo("{}: {}", key, value);
        }
        for (const auto &brush: entity.Brushes) {
            DebugInfo("BRUSH {} VERTICES, {} FACES", brush.Vertices.size(), brush.Faces.size());
        }
        DebugInfo("END ENTITY");
    }
}
