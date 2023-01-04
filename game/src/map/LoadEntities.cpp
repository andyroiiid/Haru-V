//
// Created by andyroiiid on 12/28/2022.
//

#include "map/LoadEntities.h"

#include <core/Debug.h>

#include "map/MapParser.h"
#include "map/EntityLoaders.h"

void LoadEntities(const std::string &mapFilename) {
    DebugInfo("Loading map {}.", mapFilename);
    const MapParser mapParser(mapFilename);
    LoadEntities(mapParser.GetMap());
}

typedef void (*EntityLoader)(const MapData::Entity &entity);

static const std::map<std::string, EntityLoader> s_EntityLoaders{
        {"worldspawn",        LoadWorldSpawn},
        {"info_player_start", LoadInfoPlayerStart},
        {"func_brush",        LoadFuncBrush},
        {"func_move",         LoadFuncMove},
        {"func_button",       LoadFuncButton},
        {"func_phys",         LoadFuncPhys},
        {"light_point",       LoadLightPoint},
        {"prop_test_model",   LoadPropTestModel}
};

static void LoadEntity(const MapData::Entity &entity) {
    std::string className;
    DebugCheckCritical(entity.GetPropertyString("classname", className), "Entity doesn't have a class!");

    auto loader = s_EntityLoaders.find(className);
    DebugCheckCritical(loader != s_EntityLoaders.end(), "Unknown entity type: {}", className);

    loader->second(entity);
}

void LoadEntities(const MapData::Map &map) {
    for (const MapData::Entity &entity: map.Entities) {
        LoadEntity(entity);
    }
}
