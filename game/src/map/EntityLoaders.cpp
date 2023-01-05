//
// Created by andyroiiid on 12/29/2022.
//

#include "map/EntityLoaders.h"

#include <core/Debug.h>
#include <tracy/Tracy.hpp>

#include "Globals.h"
#include "actors/AFuncBrush.h"
#include "actors/AFuncButton.h"
#include "actors/AFuncMove.h"
#include "actors/AFuncPhys.h"
#include "actors/ALightPoint.h"
#include "actors/APlayer.h"
#include "actors/APropPowerSphere.h"
#include "actors/APropTestModel.h"
#include "actors/AWorldSpawn.h"
#include "actors/Scene.h"

static void Register(Actor *actor, const MapData::Entity &entity) {
    std::string name;
    if (entity.GetPropertyString("name", name)) {
        g_Scene->Register(name, actor);
    }
}

void LoadWorldSpawn(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 lightDirection;
    DebugCheckCritical(entity.GetPropertyVector("light_direction", lightDirection), "worldspawn doesn't have a valid light_direction!");

    glm::vec3 lightColor;
    DebugCheckCritical(entity.GetPropertyColor("light_color", lightColor), "worldspawn doesn't have a valid light_color!");

    std::string script;
    DebugCheck(entity.GetPropertyString("script", script), "worldspawn doesn't have a valid script!");

    g_Scene->CreateActor<AWorldSpawn>(entity.Brushes, lightDirection, lightColor, script);
}

void LoadInfoPlayerStart(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 origin;
    DebugCheckCritical(entity.GetPropertyVector("origin", origin), "info_player_start doesn't have a valid origin!");

    g_Scene->CreateActor<APlayer>(origin);
}

void LoadFuncBrush(const MapData::Entity &entity) {
    ZoneScoped;

    g_Scene->CreateActor<AFuncBrush>(entity.Brushes);
}

void LoadFuncMove(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 moveSpeed;
    DebugCheckCritical(entity.GetPropertyVector("move_speed", moveSpeed), "func_move doesn't have a valid move_speed!");

    float moveTime;
    DebugCheckCritical(entity.GetPropertyFloat("move_time", moveTime), "func_move doesn't have a valid move_time!");

    Register(g_Scene->CreateActor<AFuncMove>(entity.Brushes, moveSpeed, moveTime), entity);
}

void LoadFuncButton(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 moveSpeed;
    DebugCheckCritical(entity.GetPropertyVector("move_speed", moveSpeed), "func_button doesn't have a valid move_speed!");

    float moveTime;
    DebugCheckCritical(entity.GetPropertyFloat("move_time", moveTime), "func_button doesn't have a valid move_time!");

    std::string event;
    DebugCheckCritical(entity.GetPropertyString("event", event), "func_button doesn't have a valid event!");

    g_Scene->CreateActor<AFuncButton>(entity.Brushes, moveSpeed, moveTime, event);
}

void LoadFuncPhys(const MapData::Entity &entity) {
    ZoneScoped;

    g_Scene->CreateActor<AFuncPhys>(entity.Brushes);
}

void LoadLightPoint(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 origin;
    DebugCheckCritical(entity.GetPropertyVector("origin", origin), "light_point doesn't have a valid origin!");

    glm::vec3 color;
    DebugCheckCritical(entity.GetPropertyColor("color", color), "light_point doesn't have a valid color!");

    float radius;
    DebugCheckCritical(entity.GetPropertyFloat("radius", radius), "light_point doesn't have a valid radius!");

    g_Scene->CreateActor<ALightPoint>(origin, color, radius);
}

void LoadPropTestModel(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 origin;
    DebugCheckCritical(entity.GetPropertyVector("origin", origin), "prop_test_model doesn't have a valid origin!");

    std::string model;
    DebugCheckCritical(entity.GetPropertyString("model", model), "prop_test_model doesn't have a valid model!");

    g_Scene->CreateActor<APropTestModel>(
        "models/" + model + ".obj", //
        "materials/" + model + ".json",
        origin
    );
}

void LoadPropPowerSphere(const MapData::Entity &entity) {
    ZoneScoped;

    glm::vec3 origin;
    DebugCheckCritical(entity.GetPropertyVector("origin", origin), "prop_power_sphere doesn't have a valid origin!");

    g_Scene->CreateActor<APropPowerSphere>(origin);
}
