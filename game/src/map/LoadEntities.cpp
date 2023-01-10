//
// Created by andyroiiid on 12/28/2022.
//

#include "map/LoadEntities.h"

#include <core/Debug.h>

#include "map/EntityLoaders.h"
#include "map/MapParser.h"

void LoadEntities(const std::string &mapFilename) {
    DebugInfo("Loading entities in map {}.", mapFilename);
    const MapParser mapParser(mapFilename);
    LoadEntities(mapParser.GetMap());
}

typedef void (*EntityLoader)(const MapData::Entity &entity);

static const std::map<std::string, EntityLoader> s_EntityLoaders{
    {"worldspawn",        LoadWorldSpawn     },
    {"info_player_start", LoadInfoPlayerStart},
    {"func_brush",        LoadFuncBrush      },
    {"func_group",        LoadFuncGroup      },
    {"func_move",         LoadFuncMove       },
    {"func_button",       LoadFuncButton     },
    {"func_phys",         LoadFuncPhys       },
    {"trigger_once",      LoadTriggerOnce    },
    {"trigger_multiple",  LoadTriggerMultiple},
    {"light_point",       LoadLightPoint     },
    {"prop_test_model",   LoadPropTestModel  },
    {"prop_power_sphere", LoadPropPowerSphere}
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
