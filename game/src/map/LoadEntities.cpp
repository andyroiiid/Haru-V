//
// Created by andyroiiid on 12/28/2022.
//

#include "map/LoadEntities.h"

#include <core/Debug.h>

#include "Globals.h"
#include "map/MapParser.h"
#include "actors/Scene.h"
#include "actors/AFuncBrush.h"
#include "actors/APlayerNoClip.h"

static void LoadWorldSpawn(const MapData::Entity &entity) {
    g_Scene->CreateActor<AFuncBrush>(entity.Brushes);
}

static void LoadInfoPlayerStart(const MapData::Entity &entity) {
    glm::vec3 origin;
    DebugCheckCritical(entity.GetPropertyVector("origin", origin), "info_player_start doesn't have a valid origin!");

    g_Scene->CreateActor<APlayerNoClip>(origin);
}

typedef void (*EntityLoader)(const MapData::Entity &entity);

static const std::map<std::string, EntityLoader> s_EntityLoaders{
        {"worldspawn",        LoadWorldSpawn},
        {"info_player_start", LoadInfoPlayerStart}
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

void LoadEntities(const std::string &mapFilename) {
    const MapParser mapParser(mapFilename);
    LoadEntities(mapParser.GetMap());
}
